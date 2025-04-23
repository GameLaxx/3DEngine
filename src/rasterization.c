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
    if (point_ptr->z <= 0){
        return EXIT_FAILURE;
    }

    ret_ptr->x = (point_ptr->x * g_context.viewportDistance) / point_ptr->z *
                 ((float)g_windowWidth / g_context.viewportWidth);
    ret_ptr->y = (point_ptr->y * g_context.viewportDistance) / point_ptr->z *
                 ((float)g_windowHeight / g_context.viewportHeight);
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

int fillTriangle(point_t* p1_ptr, point_t* p2_ptr, point_t* p3_ptr, 
    float intensityP1, float intensityP2, float intensityP3,
    rgba_t* color_ptr){
    point_t p1 = {};
    point_t p2 = {};
    point_t p3 = {};
    if(point3DtoPixel(p1_ptr, &p1) == EXIT_FAILURE || 
       point3DtoPixel(p2_ptr, &p2) == EXIT_FAILURE ||
       point3DtoPixel(p3_ptr, &p3) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    float totalArea = fabsf(triangleArea(&p1, &p2, &p3));
    if(totalArea == 0.0f){
        return EXIT_SUCCESS;
    }
    int minXs = fminf(fminf(p1.x, p2.x), p3.x);
    int maxXs = fmaxf(fmaxf(p1.x, p2.x), p3.x);
    int minYs = fminf(fminf(p1.y, p2.y), p3.y);
    int maxYs = fmaxf(fmaxf(p1.y, p2.y), p3.y);
    float areaWithout1 = 0;
    float areaWithout2 = 0;
    float areaWithout3 = 0;
    int bufferIndex = 0;
    float interpolatedZ = 0;
    float interpolatedIntensity = 0;
    rgba_t color = {};
    for (int y = minYs; y <= maxYs; y++) {
    for (int x = minXs; x <= maxXs; x++) {
        if (x < -g_xShift || y < -g_yShift || x >= g_windowWidth - g_xShift || y >= g_windowHeight - g_yShift) {
            continue;
        }
        point_t currentPoint = {.x = x + 0.5, .y = y + 0.5, .z = 0};
        areaWithout1 = triangleArea(&p2, &currentPoint, &p3);
        areaWithout2 = triangleArea(&p3, &currentPoint, &p1);
        areaWithout3 = triangleArea(&p1, &currentPoint, &p2);
        if (((areaWithout1 < 0.0f || areaWithout2 < 0.0f || areaWithout3 < 0.0f) 
            && (areaWithout1 > 0.0f || areaWithout2 > 0.0f || areaWithout3 > 0.0f))){
            continue; // pixel outside of the triangle
        }
        // remove sign from area
        areaWithout1 = fabsf(areaWithout1);
        areaWithout2 = fabsf(areaWithout2);
        areaWithout3 = fabsf(areaWithout3);
        // get z value of the current point of the triangle
        interpolatedZ = (areaWithout1/totalArea) * p1_ptr->z
                        + (areaWithout2/totalArea) * p2_ptr->z
                        + (areaWithout3/totalArea) * p3_ptr->z;
        bufferIndex = (y + g_yShift) + (x + g_xShift) * g_windowHeight;
        if(1.0f / interpolatedZ < g_context.zBuffer[bufferIndex]){
            continue; // not the closest to the camera
        }
        g_context.zBuffer[bufferIndex] = 1.0f / interpolatedZ;
        interpolatedIntensity = (areaWithout1/totalArea) * intensityP1
        + (areaWithout2/totalArea) * intensityP2
        + (areaWithout3/totalArea) * intensityP3;
        DRAW_addIntensity(color_ptr, interpolatedIntensity, &color);
        DRAW_pixel(x, y, &color);
    }
    }
    return EXIT_SUCCESS;
}

int objectSetMatrix(object_t* object_ptr){
    float rotateX_rad = object_ptr->angleRotation[0] * M_PI / 180;
    float rotateY_rad = object_ptr->angleRotation[1] * M_PI / 180;
    float rotateZ_rad = object_ptr->angleRotation[2] * M_PI / 180;
    object_ptr->rotationMatrix[0] = cos(rotateY_rad) * cos(rotateZ_rad);
    object_ptr->rotationMatrix[1] = cos(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) - sin(rotateZ_rad) * cos(rotateX_rad);
    object_ptr->rotationMatrix[2] = sin(rotateZ_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    object_ptr->rotationMatrix[3] = cos(rotateY_rad) * sin(rotateZ_rad);
    object_ptr->rotationMatrix[4] = sin(rotateZ_rad) * sin(rotateY_rad) * sin(rotateX_rad) + cos(rotateZ_rad) * cos(rotateX_rad);
    object_ptr->rotationMatrix[5] = - cos(rotateZ_rad) * sin(rotateX_rad) + sin(rotateZ_rad) * sin(rotateY_rad) * cos(rotateX_rad);
    object_ptr->rotationMatrix[6] = -sin(rotateY_rad);
    object_ptr->rotationMatrix[7] = cos(rotateY_rad) * sin(rotateX_rad);
    object_ptr->rotationMatrix[8] = cos(rotateY_rad) * cos(rotateX_rad);
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
        if(g_context.meshes[i].vertices_ptr){
            free(g_context.meshes[i].vertices_ptr);
        }
        if(g_context.meshes[i].indicesVertices_ptr){
            free(g_context.meshes[i].indicesVertices_ptr);
        }
        if(g_context.meshes[i].normals_ptr){
            free(g_context.meshes[i].normals_ptr);
        }
        if(g_context.meshes[i].indicesNormals_ptr){
            free(g_context.meshes[i].indicesNormals_ptr);
        }
    }
    return EXIT_SUCCESS;
}

int RR_drawScene(){
    g_context.zBuffer = calloc(g_windowHeight * g_windowWidth, sizeof(float));
    float intensityP1 = 0;
    float intensityP2 = 0;
    float intensityP3 = 0;
    point_t p1 = {};
    point_t p2 = {};
    point_t p3 = {};
    vector_t u = {};
    vector_t v = {};
    vector_t translateVector = {};
    vector_t normalP1 = {};
    vector_t normalP2 = {};
    vector_t normalP3 = {};
    rgba_t color = {};
    vector_t rayCenter = {};
    vector_t rayP1 = {};
    vector_t rayP2 = {};
    vector_t rayP3 = {};
    rgba_t* material_ptr = NULL;
    for(int obj = 0; obj < g_context.objectsCount; obj++){    
        objectSetMatrix(&g_context.objects[obj]);
        material_ptr = (rgba_t*)g_context.objects[obj].material_ptr;
        for(int t = 0; t < g_context.objects[obj].mesh->trianglesCount; t++){
            p1 = g_context.objects[obj].mesh->vertices_ptr[g_context.objects[obj].mesh->indicesVertices_ptr[3 * t]];
            p2 = g_context.objects[obj].mesh->vertices_ptr[g_context.objects[obj].mesh->indicesVertices_ptr[3 * t + 1]];
            p3 = g_context.objects[obj].mesh->vertices_ptr[g_context.objects[obj].mesh->indicesVertices_ptr[3 * t + 2]];
            normalP1 = g_context.objects[obj].mesh->normals_ptr[g_context.objects[obj].mesh->indicesNormals_ptr[3 * t]];
            normalP2 = g_context.objects[obj].mesh->normals_ptr[g_context.objects[obj].mesh->indicesNormals_ptr[3 * t + 1]];
            normalP3 = g_context.objects[obj].mesh->normals_ptr[g_context.objects[obj].mesh->indicesNormals_ptr[3 * t + 2]];
            COO_vectorizePoints(&g_context.origin, &g_context.objects[obj].origin, &translateVector);
            // scale the mesh
            scalePoint(&p1, g_context.objects[obj].scale);
            scalePoint(&p2, g_context.objects[obj].scale);
            scalePoint(&p3, g_context.objects[obj].scale);
            // rotate it
            rotatePoint(&p1, g_context.objects[obj].rotationMatrix);
            rotatePoint(&p2, g_context.objects[obj].rotationMatrix);
            rotatePoint(&p3, g_context.objects[obj].rotationMatrix);
            rotatePoint(&normalP1, g_context.objects[obj].rotationMatrix);
            rotatePoint(&normalP2, g_context.objects[obj].rotationMatrix);
            rotatePoint(&normalP3, g_context.objects[obj].rotationMatrix);
            // translate it
            translatePoint(&p1, &translateVector);
            translatePoint(&p2, &translateVector);
            translatePoint(&p3, &translateVector);
            // get center and geometrical normal
            point_t center = {
                .x = (p1.x + p2.x + p3.x) / 3,
                .y = (p1.y + p2.y + p3.y) / 3,
                .z = (p1.z + p2.z + p3.z) / 3,
            }; // TODO : compute center and geometrical normal in the mesh
            COO_vectorizePoints(&p1, &p2, &u);
            COO_vectorizePoints(&p1, &p3, &v);
            vector_t normal = {};
            COO_crossProduct(&u, &v, &normal);
            float normalLength = sqrt(COO_scalarProduct(&normal,&normal));
            COO_lambdaProduct(&normal, normalLength, FT_DIV);
            // ray going on the triangle
            COO_vectorizePoints(&center, &g_context.origin, &rayCenter);
            COO_vectorizePoints(&p1, &g_context.origin, &rayP1);
            COO_vectorizePoints(&p2, &g_context.origin, &rayP2);
            COO_vectorizePoints(&p3, &g_context.origin, &rayP3);
            // if(backFaceCulling(&normal, &rayCenter) == EXIT_FAILURE){
            //     continue; // is facing backward
            // }
            computeLight(&p1, &normalP1, &rayP1, 40, &intensityP1);
            computeLight(&p2, &normalP2, &rayP2, 40, &intensityP2);
            computeLight(&p3, &normalP3, &rayP3, 40, &intensityP3);
            if(g_context.objects[obj].materialType == MT_COLOR_EACH){
                fillTriangle(&p1, &p2, &p3, intensityP1, intensityP2, intensityP3, &material_ptr[t]);
            }else if(g_context.objects[obj].materialType == MT_COLOR_UNIFORM){
                fillTriangle(&p1, &p2, &p3, intensityP1, intensityP2, intensityP3, material_ptr);
            }
            intensityP1 = 0;
            intensityP2 = 0;
            intensityP3 = 0;
        }
    }
    free(g_context.zBuffer);
    g_context.zBuffer = NULL;
    return EXIT_SUCCESS;
}