#include "surface.h"
#include <stdio.h>
#include "../io/stb_image.h"

void Surface_init(Surface* surface, SDL_Surface* sdl_surface, bool mustLock) {
    surface->sdl_surface = sdl_surface;
    surface->mustLock = mustLock;
}

void Surface_freeSurface(Surface* surface) {
    if(surface->sdl_surface != NULL) {
        SDL_FreeSurface(surface->sdl_surface);
    }
}

void Surface_fill(Surface* surface, int color) {
    SDL_FillRect(surface->sdl_surface, NULL, color);
}

void Surface_fillRect(Surface* surface, int color, int x, int y, int width, int height) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;

    SDL_FillRect(surface->sdl_surface, &rect, color);
}

// void Surface_fill(Surface* surface, int color) {
//     Uint32* pixels = (Uint32*) surface->sdl_surface->pixels;
//
// 	Uint32* pCurPixel = pixels;
// 	Uint32* pLastPixel = &pixels[surface->sdl_surface->w * surface->sdl_surface->h];
// 	for(; pCurPixel < pLastPixel; pCurPixel++) {
// 		*pCurPixel = color;
// 	}
// }
//
// void Surface_fillRect(Surface* surface, int color, int x, int y, int width, int height) {
//     Uint32* pixels = (Uint32*) surface->sdl_surface->pixels;
//
//     int right = x + width;
//     int bottom = y + height;
//     int iy = y;
//     int ix = x;
//     if(y < 0) {
//         y = 0;
//         iy = 0;
//     }
//     if(bottom >= surface->sdl_surface->h) {
//         height = surface->sdl_surface->h - y;
//         bottom = y + height;
//     }
//     if(x < 0) {
//         x = 0;
//         ix = 0;
//     }
//     if(right >= surface->sdl_surface->w) {
//         width = surface->sdl_surface->w - x;
//         right = x + width;
//     }
//
//     int row = 0;
//     for(; iy < bottom; iy++) {
//         row = iy * surface->sdl_surface->w;
//         for(; ix < right; ix++) {
//             pixels[row + ix] = color;
//         }
//         ix = x;
// 	}
// }

// TODO: test this func
void Surface_blitSurface(Surface* src_surface, Surface* dest_surface, int x, int y, int width, int height) {
    Uint32* src_pixels = (Uint32*) src_surface->sdl_surface->pixels;
    Uint32* dest_pixels = (Uint32*) dest_surface->sdl_surface->pixels;

    int right = x + width;
    int bottom = y + height;
    int iy = y;
    int ix = x;
    if(y < 0) {
        y = 0;
        iy = 0;
    }
    if(bottom >= src_surface->sdl_surface->h) {
        height = src_surface->sdl_surface->h - y;
        bottom = y + height;
    }
    if(x < 0) {
        x = 0;
        ix = 0;
    }
    if(right >= src_surface->sdl_surface->w) {
        width = src_surface->sdl_surface->w - x;
        right = x + width;
    }

    int row = 0;
    int dest_row = 0;
    for(int dy = 0; iy < bottom; iy++) {
        row = iy * src_surface->sdl_surface->w;
        dest_row = dy * dest_surface->sdl_surface->w;
        for(int dx = 0; ix < right; ix++) {
            src_pixels[row + ix] = dest_pixels[dest_row + dx];
            dx++;
        }
        ix = x;
        dy++;
	}
}

// TODO: test this func
// void Surface_loadSurface(Surface* surface, const char* fileName) {
//     //load
//     int width, height, numComponents;
//     Uint32* data = (Uint32*) stbi_load(fileName, &width, &height, &numComponents, 4);
//     if(data == NULL) {
//         // TODO: handle error better
//         printf("%s%s\n", "Unable to load texture: ", fileName);
//     }
//
//     //copy
//     int nPixels = width * height;
//     Uint32* pixelData = new Uint32[nPixels];
//     for(int i = 0; i < nPixels; i++) {
//         pixelData[i] = data[i];
//     }
//     surface->sdl_surface->pixels = (void*) pixelData;
//
//     //free
//     stbi_image_free(data);
// }
