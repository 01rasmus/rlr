#include <stdlib.h>
#include <glad.h>
#include "../error.h"
#include "sampler.h"

rlr_sampler_t* rlr_sampler_create(uint32_t min, uint32_t mag, uint32_t wrap_s, uint32_t wrap_t) {
    rlr_sampler_t* sampler = malloc(sizeof(rlr_sampler_t));

    if(!sampler) {
        rlr_error_set(RLR_ERR_NO_MEMORY);
        goto err;
    }

    sampler->sampler = 0;

    glGenSamplers(1, &sampler->sampler);
    if(sampler->sampler == 0) {
        rlr_error_set(RLR_ERR_OPENGL_NULL_HANDLE);
        goto err;
    }
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_MIN_FILTER, min);
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_MAG_FILTER, mag);
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_S, wrap_s);
    glSamplerParameteri(sampler->sampler, GL_TEXTURE_WRAP_T, wrap_t);
    return sampler;
err:
    rlr_sampler_free(sampler);
    return NULL;
}

void rlr_sampler_free(rlr_sampler_t* sampler) {
    if(sampler) {
        glDeleteSamplers(1, &sampler->sampler);
    }
    free(sampler);
}