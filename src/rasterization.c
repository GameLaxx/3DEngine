//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <math.h>
#include "rasterization.h"
#include "draw.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------
sceneContext_t g_context;
//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int scalePoint(point_t* point_ptr, float scale_ptr[3]){
    point_ptr->x *= scale_ptr[0];
    point_ptr->y *= scale_ptr[1];
    point_ptr->z *= scale_ptr[2];
    return EXIT_SUCCESS;
}

int rotatePoint(point_t* point_ptr, float rotationMatrix[9]){
    float tmpX = point_ptr->x * rotationMatrix[0] + point_ptr->y * rotationMatrix[1] + point_ptr->z * rotationMatrix[2];
    float tmpY = point_ptr->x * rotationMatrix[3] + point_ptr->y * rotationMatrix[4] + point_ptr->z * rotationMatrix[5];
    float tmpZ = point_ptr->x * rotationMatrix[6] + point_ptr->y * rotationMatrix[7] + point_ptr->z * rotationMatrix[8];
    point_ptr->x = tmpX;
    point_ptr->y = tmpY;
    point_ptr->z = tmpZ;
    return EXIT_SUCCESS;
}

int translatePoint(point_t* point_ptr, vector_t* vector_ptr){
    point_ptr->x += vector_ptr->x;
    point_ptr->y += vector_ptr->y;
    point_ptr->z += vector_ptr->z;
    return EXIT_SUCCESS;
}

int backFaceCulling(vector_t* normal_ptr, vector_t* ray_ptr){
    if(COO_scalarProduct(normal_ptr, ray_ptr) <= 0){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int point3DtoPixel(point_t* point_ptr, point_t* ret_ptr){
    if(!ret_ptr || !point_ptr){
        return EXIT_FAILURE;
    }
    ret_ptr->x = (point_ptr->x * g_context.viewportDistance) / point_ptr->z * (g_windowWidth / g_context.viewportWidth);
    ret_ptr->y = (point_ptr->y * g_context.viewportDistance) / point_ptr->z * (g_windowHeight / g_context.viewportHeight);
    ret_ptr->z = g_context.viewportDistance;
    return EXIT_SUCCESS;
}

int computeLight(point_t* pointOnObject_ptr, vector_t* normal_ptr, vector_t* leavingLightVector_ptr, int specular, float* intensity){
    // leavingLightVector is the ray of light leaving the object and going to the camera. Named V.
    // commingLightVector is the ray of light leaving the object and going to the source. Named L.
    // normal vector is a unitary vector.
    for(int i = 0; i < MAX_LIGHTS; i++){
        if(g_context.lights[i].intensity <= 0) continue;
        // ambiant light just add intensity
        if(g_context.lights[i].type == LT_ambiant){
            *intensity += g_context.lights[i].intensity;
            continue;
        }
        // get direction of light
        vector_t commingLightVector = {}; 
        if(g_context.lights[i].type == LT_directional){
            if(COO_copyCoordinates(&g_context.lights[i].carac, &commingLightVector) == EXIT_FAILURE){ //! I think its in the wrond direction
                printf("*-* Error : Problem occured while getting carac of light %i.\n", i);
                return EXIT_FAILURE;
            }
        }else if(g_context.lights[i].type == LT_point){
            if(COO_vectorizePoints(pointOnObject_ptr, &g_context.lights[i].carac, &commingLightVector) == EXIT_FAILURE){
                printf("*-* Error : Problem occured while getting carac of light %i.\n", i);
                return EXIT_FAILURE;
            }
        }else{                
            printf("*-* Warning : Unknwon type of light %i : type %i.\n", i, g_context.lights[i].type);
            continue;
        }
        // get for point if shadow or not
        // float tmin = 0.000001;
        // float tmax = (g_context.lights[i].type == LT_directional) ? TMAX_ALL : TMAX_POINT;
        // object_t* closestObject_ptr = NULL;
        // float closestValue = tmax + 1;
        // float currentValue = tmax + 1;
        
        // for(int i = 0; i < MAX_OBJECTS; i++){
        //     currentValue = OBJ_intersectObject(pointOnObject_ptr, &commingLightVector, &g_context.objects[i], tmin, tmax);
        //     if(currentValue < closestValue && currentValue > tmin && currentValue < tmax){
        //         closestObject_ptr = &g_context.objects[i];
        //         closestValue = currentValue;
        //     }
        // }
        // if(closestObject_ptr != NULL){ // other object between source and current object ==> no light from this source
        //     continue;
        // }
        // precalculations
        float angleNormalLight = COO_scalarProduct(normal_ptr, &commingLightVector);
        // coeff applied to the intensity of the current light. Might be over 1.
        float coeff = 0;
        //------------- Diffuse reflection
        if(angleNormalLight >= 0){
            coeff += angleNormalLight / sqrt(COO_scalarProduct(&commingLightVector, &commingLightVector));
        }
        //------------- Specular reflection
        // Check if object is matte or shiny
        if(specular <= 0){
            *intensity += g_context.lights[i].intensity * coeff;
            continue;
        }
        vector_t reflectionVector = {};
        // compute R = 2 (N.L) N - L
        if(COO_linearTransformation(normal_ptr, 2 * angleNormalLight, &commingLightVector, -1, &reflectionVector) == EXIT_FAILURE){
            return EXIT_FAILURE;
        }
        float angleReflectionLight = COO_scalarProduct(&reflectionVector, leavingLightVector_ptr);
        if(angleReflectionLight >= 0){
            angleReflectionLight /= sqrt(COO_scalarProduct(&reflectionVector, &reflectionVector));
            angleReflectionLight /= sqrt(COO_scalarProduct(leavingLightVector_ptr, leavingLightVector_ptr));
            coeff += pow(angleReflectionLight, specular);
        }
        // add to intensity
        *intensity += g_context.lights[i].intensity * coeff;
    }
    return EXIT_SUCCESS;
}

float triangleArea(point_t* p1_ptr, point_t* p2_ptr, point_t* p3_ptr){
    return (p2_ptr->x - p1_ptr->x) * (p3_ptr->y - p1_ptr->y) - (p2_ptr->y - p1_ptr->y) * (p3_ptr->x - p1_ptr->x);
}

int fillTriangle(point_t* p1_ptr, point_t* p2_ptr, point_t* p3_ptr, rgba_t* color_ptr){
    point_t p1, p2, p3 = {};
    if(point3DtoPixel(p1_ptr, &p1) == EXIT_FAILURE || 
       point3DtoPixel(p2_ptr, &p2) == EXIT_FAILURE ||
       point3DtoPixel(p3_ptr, &p3) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    float totalArea = triangleArea(&p1, &p2, &p3);
    if(totalArea == 0){
        return EXIT_SUCCESS;
    }
    int minXs = fminf(fminf(p1.x, p2.x), p3.x);
    int maxXs = fmaxf(fmaxf(p1.x, p2.x), p3.x);
    int minYs = fminf(fminf(p1.y, p2.y), p3.y);
    int maxYs = fmaxf(fmaxf(p1.y, p2.y), p3.y);
    float areaWithout1 = 0;
    float areaWithout2 = 0;
    float areaWithout3 = 0;
    float interpolatedZ = 0;
    for (int y = minYs; y <= maxYs; y++) {
    for (int x = minXs; x <= maxXs; x++) {
        if (x < -g_xShift || y < -g_yShift || x >= g_windowWidth - g_xShift || y >= g_windowHeight - g_yShift) {
            continue;
        }
        point_t currentPoint = {.x = x + 0.5, .y = y + 0.5, .z = 0};
        areaWithout1 = triangleArea(&p2, &currentPoint, &p3);
        areaWithout2 = triangleArea(&p3, &currentPoint, &p1);
        areaWithout3 = triangleArea(&p1, &currentPoint, &p2);
        if (((areaWithout1 < 0 || areaWithout2 < 0 || areaWithout3 < 0) && (areaWithout1 > 0 || areaWithout2 > 0 || areaWithout3 > 0))){
            // pixel outside of the triangle
            continue;
        }
        interpolatedZ = (areaWithout1/totalArea) * p1_ptr->z
                        + (areaWithout2/totalArea) * p2_ptr->z
                        + (areaWithout3/totalArea) * p3_ptr->z + 0.1; // zBuffer is calloc so if == 0 then never assigned hence + 0.1
        // add shift to go in zbuffer because x and y can be < 0
        if(g_context.zBuffer[(int) ((y + g_yShift) + (x + g_xShift) * g_windowHeight)] != 0 
        && g_context.zBuffer[(int) ((y + g_yShift) + (x + g_xShift) * g_windowHeight)] < 1 / interpolatedZ){
            continue;
        }
        g_context.zBuffer[(int) ((y + g_yShift) + (x + g_xShift) * g_windowHeight)] = 1 / interpolatedZ;
        DRAW_pixel(x, y, color_ptr);
    }
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int RR_addMesh(mesh_t* mesh_ptr){
    if(g_context.meshesCount == MAX_MESHES){
        return EXIT_FAILURE;
    }
    if(mesh_ptr->id >= MAX_MESH_IDS){
        return EXIT_FAILURE;
    }
    g_context.meshesId[mesh_ptr->id] = g_context.meshesCount + 1; // calloc set at 0, so if 0 then nothing has been added ==> easier to compare hence + 1
    g_context.meshes[g_context.meshesCount] = *mesh_ptr;
    g_context.meshesCount++;
    return EXIT_SUCCESS;
}

int RR_addLight(lightSource_t* light){
    if(g_context.lightsCount == MAX_LIGHTS) return EXIT_FAILURE;
    if(light->intensity <= 0) return EXIT_FAILURE;
    for(int i = 0; i < MAX_LIGHTS; i++){
        if(g_context.lights[i].intensity <= 0){
            g_context.lights[i] = *light;
            break;
        }
        if(g_context.lights[i].type == LT_ambiant && light->type == LT_ambiant){
            g_context.lights[i].intensity += light->intensity;
            if(g_context.lights[i].intensity > 1){
                g_context.lights[i].intensity = 1;
            }
            g_context.lightsCount -= 1; //compensate the fact that we did not add a light
            break;
        }
    }
    g_context.lightsCount += 1;
    return 0;
}

int RR_addObject(object_t* object_ptr){
    if(g_context.objectsCount == MAX_OBJECTS){
        return EXIT_FAILURE;
    }
    if(object_ptr->meshId < 0 || object_ptr->meshId >= MAX_MESH_IDS){
        return EXIT_FAILURE;
    }
    if(g_context.meshesId[object_ptr->meshId] == 0){
        return EXIT_FAILURE;
    }
    g_context.objects[g_context.objectsCount] = *object_ptr;
    g_context.objects[g_context.objectsCount].mesh = &g_context.meshes[g_context.meshesId[object_ptr->meshId] - 1]; // because added with + 1
    float rotateX_rad = object_ptr->angleRotation[0] * M_PI / 180;
    float rotateY_rad = object_ptr->angleRotation[1] * M_PI / 180;
    float rotateZ_rad = object_ptr->angleRotation[2] * M_PI / 180;
    g_context.objects[g_context.objectsCount].rotationMatrix[0] = cos(rotateY_rad) * cos(rotateZ_rad);
    g_context.objects[g_context.objectsCount].rotationMatrix[1] = cos(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) - sin(rotateZ_rad) * cos(rotateX_rad);
    g_context.objects[g_context.objectsCount].rotationMatrix[2] = sin(rotateZ_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    g_context.objects[g_context.objectsCount].rotationMatrix[3] = cos(rotateY_rad) * sin(rotateZ_rad);
    g_context.objects[g_context.objectsCount].rotationMatrix[4] = sin(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * cos(rotateX_rad);
    g_context.objects[g_context.objectsCount].rotationMatrix[5] = - cos(rotateZ_rad) * sin(rotateX_rad) + sin(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    g_context.objects[g_context.objectsCount].rotationMatrix[6] = -sin(rotateY_rad);
    g_context.objects[g_context.objectsCount].rotationMatrix[7] = cos(rotateY_rad) * sin(rotateX_rad);
    g_context.objects[g_context.objectsCount].rotationMatrix[8] = cos(rotateY_rad) * cos(rotateX_rad);
    g_context.objectsCount++;
    return EXIT_SUCCESS;
}

int RR_initScene(point_t* origin, int vW, int vH, int vD){
    g_context.origin = *origin;
    g_context.viewportWidth = vW;
    g_context.viewportHeight = vH;
    g_context.viewportDistance = vD;
    g_context.meshesCount = 0;
    g_context.objectsCount = 0;
    g_context.zBuffer = NULL;
    return EXIT_SUCCESS;
}

int RR_clearScene(){
    for(int i = 0; i < g_context.meshesCount; i++){
        if(g_context.meshes[i].vertices){
            free(g_context.meshes[i].vertices);
        }
        if(g_context.meshes[i].indices){
            free(g_context.meshes[i].indices);
        }
        if(g_context.meshes[i].normalTriangles_ptr){
            free(g_context.meshes[i].normalTriangles_ptr);
        }
        if(g_context.meshes[i].normalVertices_ptr){
            free(g_context.meshes[i].normalVertices_ptr);
        }
    }
    return EXIT_SUCCESS;
}

int RR_drawScene(){
    g_context.zBuffer = calloc(g_windowHeight * g_windowWidth, sizeof(float));
    float intensity = 0;
    for(int obj = 0; obj < g_context.objectsCount; obj++){
        rgba_t* material_ptr = (rgba_t*)g_context.objects[obj].material_ptr;
        for(int t = 0; t < g_context.objects[obj].mesh->trianglesCount; t++){
            rgba_t color = {};
            point_t p1 = g_context.objects[obj].mesh->vertices[g_context.objects[obj].mesh->indices[3 * t]];
            point_t p2 = g_context.objects[obj].mesh->vertices[g_context.objects[obj].mesh->indices[3 * t + 1]];
            point_t p3 = g_context.objects[obj].mesh->vertices[g_context.objects[obj].mesh->indices[3 * t + 2]];
            scalePoint(&p1, g_context.objects[obj].scale);
            scalePoint(&p2, g_context.objects[obj].scale);
            scalePoint(&p3, g_context.objects[obj].scale);
            rotatePoint(&p1, g_context.objects[obj].rotationMatrix);
            rotatePoint(&p2, g_context.objects[obj].rotationMatrix);
            rotatePoint(&p3, g_context.objects[obj].rotationMatrix);
            translatePoint(&p1, &g_context.objects[obj].origin);
            translatePoint(&p2, &g_context.objects[obj].origin);
            translatePoint(&p3, &g_context.objects[obj].origin);
            if(g_context.objects[obj].mesh->normalTriangles_ptr){
                vector_t ray = {};
                COO_vectorizePoints(&p1, &g_context.origin, &ray);
                vector_t normalVector = g_context.objects[obj].mesh->normalTriangles_ptr[t];
                rotatePoint(&normalVector, g_context.objects[obj].rotationMatrix);
                if(backFaceCulling(&normalVector, &ray) == EXIT_FAILURE){
                    continue; // is facing backward
                }
                computeLight(&p1, &g_context.objects[obj].mesh->normalTriangles_ptr[t], &p1, 40, &intensity);
                if(g_context.objects[obj].materialType == MT_COLOR_EACH){
                    DRAW_addIntensity(&material_ptr[t], intensity, &color);
                }else if(g_context.objects[obj].materialType == MT_COLOR_UNIFORM){
                    DRAW_addIntensity(material_ptr, intensity, &color);
                }
            }else{
                if(g_context.objects[obj].materialType == MT_COLOR_EACH){
                    color = material_ptr[t];
                }else if(g_context.objects[obj].materialType == MT_COLOR_UNIFORM){
                    color = *material_ptr;
                }
            }
            fillTriangle(&p1, &p2, &p3, &color);
            intensity = 0;
        }
    }
    free(g_context.zBuffer);
    g_context.zBuffer = NULL;
    return EXIT_SUCCESS;
}