/**
 * @file   ngx_http_crun_module.c
 * @author Rodezee <rodezee@github.com>
 * @date   Wed May 17 12:06:52 2023
 *
 * @brief  A hello world module for Nginx.
 *
 * @section LICENSE
 *
 * Copyright (C) 2011 by Dominic Fallows, António P. P. Almeida <appa@perusio.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <stdio.h>
#include <string.h>
#include "docker.h"


#define DFUNCTION "hello crun\r\n"

static char *ngx_http_crun(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_crun_handler(ngx_http_request_t *r);

/**
 * This module provided directive: hello world.
 *
 */
static ngx_command_t ngx_http_crun_commands[] = {

    { ngx_string("crun"), /* directive */
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS, /* location context and takes
                                            no arguments*/
      ngx_http_crun, /* configuration setup function */
      0, /* No offset. Only one context is supported. */
      0, /* No offset when storing the module configuration on struct. */
      NULL},

    ngx_null_command /* command termination */
};

/* The hello world string. */
// static u_char ngx_crun[] = "hello test";

/* The module context. */
static ngx_http_module_t ngx_http_crun_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    NULL, /* create location configuration */
    NULL /* merge location configuration */
};

/* Module definition. */
ngx_module_t ngx_http_crun_module = {
    NGX_MODULE_V1,
    &ngx_http_crun_module_ctx, /* module context */
    ngx_http_crun_commands, /* module directives */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init master */
    NULL, /* init module */
    NULL, /* init process */
    NULL, /* init thread */
    NULL, /* exit thread */
    NULL, /* exit process */
    NULL, /* exit master */
    NGX_MODULE_V1_PADDING
};

/**
 * Content handler.
 *
 * @param r
 *   Pointer to the request structure. See http_request.h.
 * @return
 *   The status of the response generation.
 */
static ngx_int_t ngx_http_crun_handler(ngx_http_request_t *r)
{
    u_char *ngx_hello_crun;
    size_t sz;

    if ( r->args.len <= 0 ) {
        ngx_hello_crun = (u_char *) DFUNCTION;
        sz = strlen((const char*)ngx_hello_crun);
    } else {
        ngx_hello_crun = (u_char *) r->args.data;
        sz = (size_t) r->args.len;
    }

    DOCKER *docker = docker_init("v1.25");
    CURLcode response;

    if (docker)
    {
        printf("The following are the Docker images present in the system.\n");
        response = docker_get(docker, "http://v1.25/images/json");
        if (response == CURLE_OK)
        {
        fprintf(stderr, "%s\n", docker_buffer(docker));
        }

        docker_destroy(docker);
        ngx_hello_crun = (u_char *) "The following are the Docker images present in the system.\n";
    } 
    else 
    {
        fprintf(stderr, "ERROR: Failed to get get a docker client!\n");
        ngx_hello_crun = (u_char *) "ERROR: Failed to get get a docker client!\n";
    }

    r->headers_out.content_type.len = strlen("text/html");
    r->headers_out.content_type.data = (u_char *) "text/html";
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = sz;
    ngx_http_send_header(r);

    ngx_buf_t    *b;
    ngx_chain_t   *out;

    b = ngx_calloc_buf(r->pool);

    out = ngx_alloc_chain_link(r->pool);

    out->buf = b;
    out->next = NULL;

    b->pos = ngx_hello_crun;
    b->last = ngx_hello_crun + sz;
    b->memory = 1;
    b->last_buf = 1;

    return ngx_http_output_filter(r, out);

} /* ngx_http_crun_handler */

/**
 * Configuration setup function that installs the content handler.
 *
 * @param cf
 *   Module configuration structure pointer.
 * @param cmd
 *   Module directives structure pointer.
 * @param conf
 *   Module configuration structure pointer.
 * @return string
 *   Status of the configuration setup.
 */
static char *ngx_http_crun(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf; /* pointer to core location configuration */

    /* Install the hello world handler. */
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_crun_handler;

    return NGX_CONF_OK;
} /* ngx_http_crun */

