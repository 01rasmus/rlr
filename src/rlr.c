#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_ds.h>
#include "backends/backend.h"
#include "resources/model_static.h"
#include "resources/cube_map.h"
#include "resources/uniform.h"
#include "resources/shader.h"
#include "resources/font.h"
#include "objects/label.h"
#include "error.h"
#include "rlr.h"

typedef struct uniform_ui_t {
    float inv_x;
    float inv_y;
} uniform_ui_t;

typedef struct uniform_model_t {
    mat4_t model;
    mat4_t mvp;
    vec3_t camera_pos;
} uniform_model_t;

typedef struct uniform_environment_t {
    vec3_t light_direction;
    vec3_t ambient_light_color;
    float ambient_light_strength;
} uniform_environment_t;

static rlr_t* _rlr = NULL;

const char mtsdf_fragment[] = "#version 330\n"
"in vec2 frag_uv;\n"
"in vec3 frag_color;\n"
"out vec4 final_color;\n"
"uniform sampler2D tex;\n"
"float median(float r, float g, float b) {\n"
"    return max(min(r,g), min(max(r,g),b));\n"
"}\n"
"float screen_px_range() {\n"
"    float px_range = 2.0;\n"
"    vec2 unit_range = vec2(px_range) / vec2(textureSize(tex, 0));\n"
"    vec2 screen_tex_size = vec2(1.0) / fwidth(frag_uv);\n"
"    return max(0.5 * dot(unit_range, screen_tex_size), 1.0);\n"
"}\n"
"void main() {\n"
"    vec3 msd = texture(tex, frag_uv).rgb;\n"
"    float sd = median(msd.r, msd.g, msd.b);\n"
"    float screen_px_distance = screen_px_range() * (sd - 0.5);\n"
"    float alpha = clamp(screen_px_distance + 0.5, 0.0, 1.0);\n"
"    final_color = vec4(frag_color, alpha);\n"
"}\n";

const char mtsdf_vertex[] = "#version 330\n"
"layout (location = 0) in vec3 color;\n"
"layout (location = 1) in vec2 pos;\n"
"layout (location = 2) in vec2 uv;\n"
"layout(std140) uniform ui {\n"
"   float inv_x;\n"
"   float inv_y;\n"
"};\n"
"out vec2 frag_uv;\n"
"out vec3 frag_color;\n"
"void main() {\n"
"frag_uv = uv;\n"
"frag_color = color;\n"
"gl_Position = vec4(pos.x * inv_x * 2.0 - 1.0, 1.0 - pos.y * inv_y * 2.0, 0.0, 1.0);\n"
"}\n";

const char model_fragment[] = RLR_SHADER_INLINE(
    in vec2 frag_uv;
    in vec3 frag_normal;
    in vec3 frag_vert_pos;
    out vec4 out_color;

    uniform sampler2D tex;
    uniform samplerCube cube_map;

    layout(std140) uniform ubo_model {
        mat4 model;
        mat4 mvp;
        vec3 camera_pos;
    };

    layout(std140) uniform ubo_material {
        vec4 color;
        float shininess;
        float specular_strength;
        float reflectiveness;
    };

    const vec3 light_pos = vec3(0, 5000.0, 0);
    const vec3 ambient_color = vec3(0.2, 0.5, 0.4);

    void main() {
        vec4 tex_color = texture(tex, frag_uv);
        vec3 base_color = tex_color.rgb * color.rgb;

        vec3 N = normalize(frag_normal);
        vec3 L = normalize(light_pos - frag_vert_pos);
        vec3 V = normalize(camera_pos - frag_vert_pos);
        vec3 H = normalize(L + V);

        vec3 R = normalize(reflect(-V, N));
        vec3 reflected_color = texture(cube_map, R).rgb;

        vec3 diffuse_color = base_color;
        vec3 spec_color = vec3(1.0);

        float ambient_strength = 0.15;
        vec3 ambient = base_color * ambient_color * ambient_strength;

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diffuse_color * diff;

        float spec = pow(max(dot(N, H), 0.0), shininess);
        vec3 specular = spec_color * spec * specular_strength;

        vec3 lit_color = ambient + diffuse + specular;
        vec3 final_rgb = mix(lit_color, reflected_color, reflectiveness);
        out_color = vec4(final_rgb, tex_color.a * color.a);
    }
);

const char model_vertex[] = RLR_SHADER_INLINE(
    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;

    layout(std140) uniform ubo_model {
        mat4 model;
        mat4 mvp;
        vec3 camera_pos;
    };

    out vec2 frag_uv;
    out vec3 frag_normal;
    out vec3 frag_vert_pos;

    void main() {
        vec4 world_pos = model * vec4(pos, 1.0);
        frag_vert_pos = world_pos.xyz;
        frag_normal = mat3(transpose(inverse(model))) * normal;
        frag_uv = uv;
        gl_Position = mvp * vec4(pos, 1.0);
    }
);

void rlr_init(const char* title, uint32_t window_width, uint32_t window_height, uint64_t flags) {
    _rlr = NULL;

    _rlr = malloc(sizeof(rlr_t));
    if(!_rlr) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    _rlr->backend = NULL;
    _rlr->window = NULL;
    _rlr->shader_text = NULL;
    _rlr->obj_labels = NULL;

    _rlr->framebuffer_width = window_width;
    _rlr->framebuffer_height = window_height;

    if(!glfwInit()) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    _rlr->window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
    if(!_rlr->window) {
        rlr_error_set(RLR_ERR_WINDOW_CREATION);
        goto err;
    }
    glfwMakeContextCurrent(_rlr->window);
    glfwSwapInterval(0);

    rlr_backend_t* backend = rlr_backend_gl3((rlr_backend_loader_t)glfwGetProcAddress);
    _rlr->backend = backend;

    // if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     rlr_error_set(RLR_ERR_CONTEXT_CREATION);
    //     goto err;
    // }

    // int32_t major = 0;
    // int32_t minor = 0;
    // glGetIntegerv(GL_MAJOR_VERSION, &major); 
    // glGetIntegerv(GL_MINOR_VERSION, &minor);
    // bool is_compatible_version = (major == 3 && minor >= 3) || major == 4;
    // if(!is_compatible_version) {
    //     rlr_error_setf(RLR_ERR_OPENGL_INCOMPATIBLE_VERSION, "expected atleast OpenGL 3.3, but got OpenGL %d.%d", major, minor);
    //     goto err;
    // }

    _rlr->test = rlr_model_static_create("assets/plane.glb");
    //_rlr->test_cube_map = rlr_cube_map_load("assets/skybox/right.jpg", "assets/skybox/left.jpg", "assets/skybox/top.jpg", "assets/skybox/bottom.jpg", "assets/skybox/front.jpg", "assets/skybox/back.jpg");
    _rlr->test_cube_map = rlr_cube_map_load(
        "assets/s/px.png",
        "assets/s/nx.png",
        "assets/s/py.png",
        "assets/s/ny.png",
        "assets/s/pz.png",
        "assets/s/nz.png"
    );
    rlr_cube_map_bind(_rlr->test_cube_map, 4);

    _rlr->texture_white = rlr_texture_default();

    _rlr->shader_text = rlr_shader_create(mtsdf_vertex, mtsdf_fragment);
    rlr_shader_bind_uniform_slot(_rlr->shader_text, "ui", 0);

    _rlr->shader_model = rlr_shader_create(model_vertex, model_fragment);
    rlr_shader_bind_uniform_slot(_rlr->shader_model, "ubo_model", 1);
    rlr_shader_bind_uniform_slot(_rlr->shader_model, "ubo_material", 2);
    rlr_shader_bind_texture_slot(_rlr->shader_model, "tex", 0);
    rlr_shader_bind_texture_slot(_rlr->shader_model, "cube_map", 4);

    uniform_ui_t ubo_ui = {
        .inv_x = 1.0 / (float)window_width,
        .inv_y = 1.0 / (float)window_height
    };

    _rlr->ubo_ui = rlr_uniform_create_dynamic(sizeof(uniform_ui_t));
    rlr_uniform_update(_rlr->ubo_ui, 0, &ubo_ui, sizeof(uniform_ui_t));
    rlr_uniform_bind(_rlr->ubo_ui, 0);

    _rlr->ubo_material = rlr_uniform_create_dynamic(sizeof(rlr_material_t));
    rlr_uniform_bind(_rlr->ubo_material, 2);

    vec3_t cam_pos = vec3_mulf(vec3(-0.05, 0.1, -0.1), 7);
    vec3_t scene_center = vec3(0, 0, 0);
    vec3_t up = vec3(0, 1, 0);
    mat4_t projection = mat4_perspective(1, (float)window_width / (float)window_height, 0.001, 100.0);
    mat4_t view = mat4_look_at(&cam_pos, &scene_center, &up);
    uniform_model_t ubo_model = {
        .mvp = mat4_mul(&projection, &view),
        .model = mat4_ident,
        .camera_pos = cam_pos,
    };

    _rlr->ubo_model = rlr_uniform_create_dynamic(sizeof(uniform_model_t));
    rlr_uniform_update(_rlr->ubo_model, 0, &ubo_model, sizeof(uniform_model_t));
    rlr_uniform_bind(_rlr->ubo_model, 1);

    return;
err:
    rlr_free();
    return;
}

void rlr_free() {
    if(!_rlr) {
        return;
    }

    //remove objects
    for(int64_t i = 0; i < arrlen(_rlr->obj_labels); i++) {
        rlr_obj_label_free(&_rlr->obj_labels[i]);
    }
    arrfree(_rlr->obj_labels);

    //free resources
    rlr_texture_free(_rlr->texture_white);
    rlr_uniform_free(_rlr->ubo_ui);
    rlr_uniform_free(_rlr->ubo_material);
    rlr_uniform_free(_rlr->ubo_model);
    rlr_shader_free(_rlr->shader_text);
    rlr_shader_free(_rlr->shader_model);

    //free backend API and window
    if(_rlr->backend) {
        _rlr->backend->backend_free();
        free(_rlr->backend);
    }
    if(_rlr->window) {
        glfwDestroyWindow(_rlr->window);
    }
    free(_rlr);
    glfwTerminate();
    _rlr = NULL;
}

double rlr_time() {
    return glfwGetTime();
}

bool rlr_draw() {
    glfwPollEvents();

    if(glfwWindowShouldClose(_rlr->window)) {
        return false;
    }

    int32_t width = 0;
    int32_t height = 0;
    glfwGetFramebufferSize(_rlr->window, &width, &height);
    if(width != _rlr->framebuffer_width || height != _rlr->framebuffer_height) {
        uniform_ui_t ubo_ui = {
            .inv_x = 1.0 / (float)width,
            .inv_y = 1.0 / (float)height
        };
        rlr_uniform_update(_rlr->ubo_ui, 0, &ubo_ui, sizeof(uniform_ui_t));
        rlr_backend()->viewport_set(0, 0, width, height);
    }

    _rlr->backend->clear_color(0.1, 0.2, 0.3, 1.0);
    _rlr->backend->clear(RLR_BACKEND_CLEAR_BIT_COLOR | RLR_BACKEND_CLEAR_BIT_DEPTH);

    //render test monkey
    rlr_shader_use(_rlr->shader_model);
    rlr_backend()->depth_testing_set(true);
    for(int64_t i = 0; i < arrlen(_rlr->test->meshes); i++) {
        rlr_mesh_static_t* mesh = &_rlr->test->meshes[i];
        rlr_uniform_update(_rlr->ubo_material, 0, &mesh->material, sizeof(rlr_material_t));
        if(mesh->texture_base) {
            rlr_texture_bind(mesh->texture_base, 0);
        } else {
            rlr_texture_bind(_rlr->texture_white, 0);
        }
        rlr_backend()->vertex_array_bind(mesh->vao);
        rlr_backend()->draw_elements(0, mesh->index_count, RLR_BACKEND_BUFFER_TYPE_U32);
    }

    //render objects
    rlr_shader_use(_rlr->shader_text);
    rlr_backend()->depth_testing_set(false);
    for(int64_t i = 0; i < arrlen(_rlr->obj_labels); i++) {
        rlr_obj_label_t* label = &_rlr->obj_labels[i];
        if(!label->visible) {
            continue;
        }
        rlr_texture_bind(label->font->texture, 0);
        rlr_backend()->vertex_array_bind(label->vao);
        rlr_backend()->draw_array(0, label->vertex_count);
    }

    glfwSwapBuffers(_rlr->window);
    return true;
}

rlr_backend_t* rlr_backend() {
    return _rlr->backend;
}

rlr_t* _rlr_raw() {
    return _rlr;
}