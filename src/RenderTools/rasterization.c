//-----------------------------------------------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------------------------------------------
#include <dirent.h>
#include <math.h>
#include "rasterization.h"
#include "draw.h"
//-----------------------------------------------------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------------------------------------------------
int getAllMeshVariables(renderContext_t* context_ptr){
    const char *dossier = "./meshes/";
    struct dirent *ent;
    DIR *dir = opendir(dossier);

    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 && context_ptr->meshesCount < MAX_MESHES) {
            char* filePath_ptr = calloc(strlen(dossier) + strlen(ent->d_name), sizeof(char));
            strcpy(filePath_ptr, dossier);
            strcat(filePath_ptr, ent->d_name);
            mesh_t currentMesh = {};
            OBJ_readObjFile(filePath_ptr, context_ptr->meshesCount, &currentMesh);
            RR_addMesh(&currentMesh, context_ptr);
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
}

int objectSetMatrix(object_t* object_ptr, renderContext_t* context_ptr){
    float rotateX_rad = (object_ptr->angleRotation[0] + context_ptr->angleRotation[0]) * M_PI / 180;
    float rotateY_rad = (object_ptr->angleRotation[1] + context_ptr->angleRotation[1]) * M_PI / 180;
    float rotateZ_rad = (object_ptr->angleRotation[2] + context_ptr->angleRotation[2]) * M_PI / 180;
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

int computeCenter(point_t* p1_ptr, point_t* p2_ptr, point_t* p3_ptr, point_t* ret_ptr){
    ret_ptr->x = (p1_ptr->x + p2_ptr->x + p3_ptr->x) / 3;
    ret_ptr->y = (p1_ptr->y + p2_ptr->y + p3_ptr->y) / 3;
    ret_ptr->z = (p1_ptr->z + p2_ptr->z + p3_ptr->z) / 3;
    return EXIT_SUCCESS;
}

int computeNormal(point_t* p1_ptr, point_t* p2_ptr, point_t* p3_ptr, vector_t* ret_ptr){
    vector_t u = {};
    vector_t v = {};
    COO_vectorizePoints(p1_ptr, p2_ptr, &u);
    COO_vectorizePoints(p1_ptr, p3_ptr, &v);
    COO_crossProduct(&u, &v, ret_ptr);
    float normalLength = sqrt(COO_scalarProduct(ret_ptr, ret_ptr));
    if(normalLength == 0){
        return EXIT_FAILURE;
    }
    COO_lambdaProduct(ret_ptr, normalLength, FT_DIV);
    return EXIT_SUCCESS;
}

int backFaceCulling(vector_t* normal_ptr, vector_t* ray_ptr){
    if(COO_scalarProduct(normal_ptr, ray_ptr) > 0){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int point3DtoPixel(point_t* point_ptr, renderContext_t* context_ptr, point_t* ret_ptr){
    if(!ret_ptr || !point_ptr){
        return EXIT_FAILURE;
    }
    if (point_ptr->z <= 0){
        return EXIT_FAILURE;
    }

    ret_ptr->x = (point_ptr->x * context_ptr->viewportDistance) / point_ptr->z *
                 ((float)g_pixelWidth / context_ptr->viewportWidth);
    ret_ptr->y = (point_ptr->y * context_ptr->viewportDistance) / point_ptr->z *
                 ((float)g_pixelHeight / context_ptr->viewportHeight);
    ret_ptr->z = context_ptr->viewportDistance;
    return EXIT_SUCCESS;
}


int computeLight(renderContext_t* context_ptr, point_t* pointOnObject_ptr, vector_t* normal_ptr, vector_t* leavingLightVector_ptr, int specular, float* intensity){
    // leavingLightVector is the ray of light leaving the object and going to the camera. Named V.
    // commingLightVector is the ray of light leaving the object and going to the source. Named L.
    // normal vector is a unitary vector.
    for(int i = 0; i < MAX_LIGHTS; i++){
        if(context_ptr->lights[i].intensity <= 0) continue;
        // ambiant light just add intensity
        if(context_ptr->lights[i].type == LT_ambiant){
            *intensity += context_ptr->lights[i].intensity;
            continue;
        }
        // get direction of light
        vector_t commingLightVector = {}; 
        if(context_ptr->lights[i].type == LT_directional){
            if(COO_copyCoordinates(&context_ptr->lights[i].carac, &commingLightVector) == EXIT_FAILURE){ //! I think its in the wrond direction
                printf("*-* Error : Problem occured while getting carac of light %i.\n", i);
                return EXIT_FAILURE;
            }
        }else if(context_ptr->lights[i].type == LT_point){
            if(COO_vectorizePoints(pointOnObject_ptr, &context_ptr->lights[i].carac, &commingLightVector) == EXIT_FAILURE){
                printf("*-* Error : Problem occured while getting carac of light %i.\n", i);
                return EXIT_FAILURE;
            }
        }else{                
            printf("*-* Warning : Unknwon type of light %i : type %i.\n", i, context_ptr->lights[i].type);
            continue;
        }
        // get for point if shadow or not
        // float tmin = 0.000001;
        // float tmax = (context_ptr->lights[i].type == LT_directional) ? TMAX_ALL : TMAX_POINT;
        // object_t* closestObject_ptr = NULL;
        // float closestValue = tmax + 1;
        // float currentValue = tmax + 1;
        
        // for(int i = 0; i < MAX_OBJECTS; i++){
        //     currentValue = OBJ_intersectObject(pointOnObject_ptr, &commingLightVector, &context_ptr->objects[i], tmin, tmax);
        //     if(currentValue < closestValue && currentValue > tmin && currentValue < tmax){
        //         closestObject_ptr = &context_ptr->objects[i];
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
            *intensity += context_ptr->lights[i].intensity * coeff;
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
        *intensity += context_ptr->lights[i].intensity * coeff;
    }
    return EXIT_SUCCESS;
}

float triangleArea(point_t* p1_ptr, point_t* p2_ptr, point_t* p3_ptr){
    return (p2_ptr->x - p1_ptr->x) * (p3_ptr->y - p1_ptr->y) - (p2_ptr->y - p1_ptr->y) * (p3_ptr->x - p1_ptr->x);
}

int fillTriangle(point_t* p1_ptr, point_t* p2_ptr, point_t* p3_ptr, 
    float intensityP1, float intensityP2, float intensityP3,
    rgba_t* color_ptr, renderContext_t* context_ptr){
    point_t p1 = {};
    point_t p2 = {};
    point_t p3 = {};
    if(point3DtoPixel(p1_ptr, context_ptr, &p1) == EXIT_FAILURE || 
       point3DtoPixel(p2_ptr, context_ptr, &p2) == EXIT_FAILURE ||
       point3DtoPixel(p3_ptr, context_ptr, &p3) == EXIT_FAILURE){
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
        if(1.0f / interpolatedZ < context_ptr->zBuffer[bufferIndex]){
            continue; // not the closest to the camera
        }
        context_ptr->zBuffer[bufferIndex] = 1.0f / interpolatedZ;
        interpolatedIntensity = (areaWithout1/totalArea) * intensityP1
        + (areaWithout2/totalArea) * intensityP2
        + (areaWithout3/totalArea) * intensityP3;
        DRAW_addIntensity(color_ptr, interpolatedIntensity, &color);
        DRAW_pixel(x, y, &color);
    }
    }
    return EXIT_SUCCESS;
}

int clipLine(point_t* p1_ptr, point_t* p2_ptr, vector_t* plane_ptr, float offset){
    float d1 = COO_scalarProduct(p1_ptr, plane_ptr) - offset;
    float d2 = COO_scalarProduct(p2_ptr, plane_ptr) - offset;
    if(d1 < 0 && d2 < 0){
        return EXIT_FAILURE;
    }
    if(d1 >= 0 && d2 >= 0){
        return EXIT_SUCCESS;
    }
    float t = d1 / (d1 - d2); // d1 != d2 because conditions above
    point_t intersection = {};
    COO_linearTransformation(p1_ptr, 1 - t, p2_ptr, t, &intersection);
    if(d1 >= 0){ // p2 is behind the plane
        p2_ptr->x = intersection.x;
        p2_ptr->y = intersection.y;
        p2_ptr->z = intersection.z;
    }else{
        p1_ptr->x = intersection.x;
        p1_ptr->y = intersection.y;
        p1_ptr->z = intersection.z;
    }
    return EXIT_SUCCESS;
}

int drawGrid(point_t* p1World_ptr, point_t* p2World_ptr, vector_t* translateVector_ptr, rgba_t* color_ptr, renderContext_t* context_ptr){
    vector_t frontPlan = {.z = 1};
    vector_t backPlan = {.z = -1};
    vector_t rightPlan = {.x = -0.7071, .z = 0.7071};
    vector_t leftPlan = {.x = 0.7071, .z = 0.7071};
    vector_t topPlan = {.y = -0.7071, .z = 0.7071};
    vector_t bottomPlan = {.y = 0.7071, .z = 0.7071};
    COO_translatePoint(p1World_ptr, translateVector_ptr);
    COO_translatePoint(p2World_ptr, translateVector_ptr);
    COO_rotationVectorProduct(p1World_ptr, context_ptr->angleRotation[0], context_ptr->angleRotation[1], context_ptr->angleRotation[2]);
    COO_rotationVectorProduct(p2World_ptr, context_ptr->angleRotation[0], context_ptr->angleRotation[1], context_ptr->angleRotation[2]);
    // clip front plan
    if(clipLine(p1World_ptr, p2World_ptr, &frontPlan, context_ptr->viewportDistance) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip back plan
    if(clipLine(p1World_ptr, p2World_ptr, &backPlan, -context_ptr->renderDistance) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip left plan
    if(clipLine(p1World_ptr, p2World_ptr, &leftPlan, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip right plan
    if(clipLine(p1World_ptr, p2World_ptr, &rightPlan, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip top plan
    if(clipLine(p1World_ptr, p2World_ptr, &topPlan, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    // clip bottom plan
    if(clipLine(p1World_ptr, p2World_ptr, &bottomPlan, 0) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    point_t p1Pixel = {};
    point_t p2Pixel = {};
    if(point3DtoPixel(p1World_ptr, context_ptr, &p1Pixel) == EXIT_FAILURE || 
    point3DtoPixel(p2World_ptr, context_ptr, &p2Pixel) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    DRAW_line(p1Pixel.x, p1Pixel.y, p2Pixel.x, p2Pixel.y, color_ptr); // TODO : replace draw line by draw each pixel with zbuffer
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------------------------------------------------
int RR_addMesh(mesh_t* mesh_ptr, renderContext_t* context_ptr){
    if(context_ptr->meshesCount == MAX_MESHES){
        return EXIT_FAILURE;
    }
    if(mesh_ptr->id >= MAX_MESH_IDS){
        return EXIT_FAILURE;
    }
    context_ptr->meshesId[mesh_ptr->id] = context_ptr->meshesCount + 1; // calloc set at 0, so if 0 then nothing has been added ==> easier to compare hence + 1
    context_ptr->meshes[context_ptr->meshesCount] = *mesh_ptr;
    context_ptr->meshesCount++;
    return EXIT_SUCCESS;
}

int RR_addLight(lightSource_t* light, renderContext_t* context_ptr){
    if(context_ptr->lightsCount == MAX_LIGHTS) return EXIT_FAILURE;
    if(light->intensity <= 0) return EXIT_FAILURE;
    for(int i = 0; i < MAX_LIGHTS; i++){
        if(context_ptr->lights[i].intensity <= 0){
            context_ptr->lights[i] = *light;
            break;
        }
        if(context_ptr->lights[i].type == LT_ambiant && light->type == LT_ambiant){
            context_ptr->lights[i].intensity += light->intensity;
            if(context_ptr->lights[i].intensity > 1){
                context_ptr->lights[i].intensity = 1;
            }
            context_ptr->lightsCount -= 1; // compensate the fact that we did not add a light
            break;
        }
    }
    context_ptr->lightsCount += 1;
    return 0;
}

int RR_addObject(object_t* object_ptr, renderContext_t* context_ptr){
    if(context_ptr->objectsCount == MAX_OBJECTS){
        return EXIT_FAILURE;
    }
    if(object_ptr->meshId < 0 || object_ptr->meshId >= MAX_MESH_IDS){
        return EXIT_FAILURE;
    }
    if(context_ptr->meshesId[object_ptr->meshId] == 0){
        return EXIT_FAILURE;
    }
    context_ptr->objects[context_ptr->objectsCount] = *object_ptr;
    context_ptr->objects[context_ptr->objectsCount].mesh = &context_ptr->meshes[context_ptr->meshesId[object_ptr->meshId] - 1]; // because added with + 1
    context_ptr->objectsCount++;
    return EXIT_SUCCESS;
}

int RR_initScene(point_t* origin_ptr, int viewportWidth, int viewportHeight, int viewportDistance, int renderDistance, renderContext_t* context_ptr){
    context_ptr->origin = *origin_ptr;
    context_ptr->viewportWidth = viewportWidth;
    context_ptr->viewportHeight = viewportHeight;
    context_ptr->viewportDistance = viewportDistance;
    context_ptr->renderDistance = renderDistance;
    context_ptr->meshesCount = 0;
    context_ptr->objectsCount = 0;
    context_ptr->zBuffer = NULL;
    getAllMeshVariables(context_ptr);
    return EXIT_SUCCESS;
}

int RR_clearScene(renderContext_t* context_ptr){
    for(int i = 0; i < context_ptr->meshesCount; i++){
        if(context_ptr->meshes[i].vertices_ptr){
            free(context_ptr->meshes[i].vertices_ptr);
        }
        if(context_ptr->meshes[i].indicesVertices_ptr){
            free(context_ptr->meshes[i].indicesVertices_ptr);
        }
        if(context_ptr->meshes[i].normals_ptr){
            free(context_ptr->meshes[i].normals_ptr);
        }
        if(context_ptr->meshes[i].indicesNormals_ptr){
            free(context_ptr->meshes[i].indicesNormals_ptr);
        }
    }
    return EXIT_SUCCESS;
}

int RR_renderObjects(renderContext_t* context_ptr){
    context_ptr->zBuffer = calloc(g_windowHeight * g_windowWidth, sizeof(float));
    float intensityP1 = 0;
    float intensityP2 = 0;
    float intensityP3 = 0;
    point_t p1 = {};
    point_t p2 = {};
    point_t p3 = {};
    point_t center = {};
    vector_t translateVector = {};
    vector_t normalP1 = {};
    vector_t normalP2 = {};
    vector_t normalP3 = {};
    vector_t geometricalNormal = {};
    vector_t rayP1 = {};
    vector_t rayP2 = {};
    vector_t rayP3 = {};
    rgba_t* material_ptr = NULL;
    for(int obj = 0; obj < context_ptr->objectsCount; obj++){    
        objectSetMatrix(&context_ptr->objects[obj], context_ptr);
        material_ptr = (rgba_t*)context_ptr->objects[obj].material_ptr;
        for(int t = 0; t < context_ptr->objects[obj].mesh->trianglesCount; t++){
            p1 = context_ptr->objects[obj].mesh->vertices_ptr[context_ptr->objects[obj].mesh->indicesVertices_ptr[3 * t]];
            p2 = context_ptr->objects[obj].mesh->vertices_ptr[context_ptr->objects[obj].mesh->indicesVertices_ptr[3 * t + 1]];
            p3 = context_ptr->objects[obj].mesh->vertices_ptr[context_ptr->objects[obj].mesh->indicesVertices_ptr[3 * t + 2]];
            normalP1 = context_ptr->objects[obj].mesh->normals_ptr[context_ptr->objects[obj].mesh->indicesNormals_ptr[3 * t]];
            normalP2 = context_ptr->objects[obj].mesh->normals_ptr[context_ptr->objects[obj].mesh->indicesNormals_ptr[3 * t + 1]];
            normalP3 = context_ptr->objects[obj].mesh->normals_ptr[context_ptr->objects[obj].mesh->indicesNormals_ptr[3 * t + 2]];
            COO_vectorizePoints(&context_ptr->origin, &context_ptr->objects[obj].origin, &translateVector);
            COO_rotationVectorProduct(&translateVector, context_ptr->angleRotation[0], context_ptr->angleRotation[1], context_ptr->angleRotation[2]);
            // scale the mesh
            scalePoint(&p1, context_ptr->objects[obj].scale);
            scalePoint(&p2, context_ptr->objects[obj].scale);
            scalePoint(&p3, context_ptr->objects[obj].scale);
            // rotate it
            rotatePoint(&p1, context_ptr->objects[obj].rotationMatrix);
            rotatePoint(&p2, context_ptr->objects[obj].rotationMatrix);
            rotatePoint(&p3, context_ptr->objects[obj].rotationMatrix);
            rotatePoint(&normalP1, context_ptr->objects[obj].rotationMatrix);
            rotatePoint(&normalP2, context_ptr->objects[obj].rotationMatrix);
            rotatePoint(&normalP3, context_ptr->objects[obj].rotationMatrix);
            // translate it
            COO_translatePoint(&p1, &translateVector);
            COO_translatePoint(&p2, &translateVector);
            COO_translatePoint(&p3, &translateVector);
            // get center and geometrical normal
            computeCenter(&p1, &p2, &p3, &center);
            computeNormal(&p1, &p2, &p3, &geometricalNormal);
            // ray going on the triangle
            if(backFaceCulling(&geometricalNormal, &center) == EXIT_FAILURE){
                continue; // is facing backward
            }
            // normal of each point
            COO_vectorizePoints(&p1, &context_ptr->origin, &rayP1);
            COO_vectorizePoints(&p2, &context_ptr->origin, &rayP2);
            COO_vectorizePoints(&p3, &context_ptr->origin, &rayP3);
            // light intensity on each point
            computeLight(context_ptr, &p1, &normalP1, &rayP1, 40, &intensityP1);
            computeLight(context_ptr, &p2, &normalP2, &rayP2, 40, &intensityP2);
            computeLight(context_ptr, &p3, &normalP3, &rayP3, 40, &intensityP3);
            // fill whole triangle
            if(context_ptr->objects[obj].materialType == MT_COLOR_EACH){
                fillTriangle(&p1, &p2, &p3, intensityP1, intensityP2, intensityP3, &material_ptr[t], context_ptr);
            }else if(context_ptr->objects[obj].materialType == MT_COLOR_UNIFORM){
                fillTriangle(&p1, &p2, &p3, intensityP1, intensityP2, intensityP3, material_ptr, context_ptr);
            }
            // reset intensity
            intensityP1 = 0;
            intensityP2 = 0;
            intensityP3 = 0;
        }
    }
    free(context_ptr->zBuffer);
    context_ptr->zBuffer = NULL;
    return EXIT_SUCCESS;
}

int RR_renderGrids(renderContext_t* context_ptr){
    vector_t translateVector = {};
    COO_linearTransformation(&context_ptr->origin, -1, NULL, 0, &translateVector);
    rgba_t gray = {211,211,211,0};
    rgba_t red = {.red = 255};
    rgba_t green = {.green = 255};
    for(int z = context_ptr->origin.z - context_ptr->renderDistance; z <= context_ptr->origin.z + context_ptr->renderDistance; z++){
        // XZ
        point_t p1WorldXZ = {.x = context_ptr->origin.x - context_ptr->renderDistance, .y = 0, .z = z};
        point_t p2WorldXZ = {.x = context_ptr->origin.x + context_ptr->renderDistance, .y = 0, .z = z};
        drawGrid(&p1WorldXZ, &p2WorldXZ, &translateVector, &gray, context_ptr);
        // // YZ
        // point_t p1WorldYZ = {.x = 0, .y = context_ptr->origin.y - context_ptr->renderDistance, .z = z};
        // point_t p2WorldYZ = {.x = 0, .y = context_ptr->origin.y + context_ptr->renderDistance, .z = z};
        // drawGrid(&p1WorldYZ, &p2WorldYZ, &translateVector, &black, context_ptr);
    }
    // for(int y = context_ptr->origin.y - context_ptr->renderDistance; y <= context_ptr->origin.y + context_ptr->renderDistance; y++){
    //     // YZ plan
    //     point_t p1WorldYZ = {.x = 0, .y = y, .z = context_ptr->origin.z - context_ptr->renderDistance};
    //     point_t p2WorldYZ = {.x = 0, .y = y, .z = context_ptr->origin.z + context_ptr->renderDistance};
    //     drawGrid(&p1WorldYZ, &p2WorldYZ, &translateVector, &black, context_ptr);
    //     // XY plan
    //     point_t p1WorldXY = {.x = context_ptr->origin.x - context_ptr->renderDistance, .y = y, .z = 0};
    //     point_t p2WorldXY = {.x = context_ptr->origin.x + context_ptr->renderDistance, .y = y, .z = 0};
    //     drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, (y == 0) ? &red : &black, context_ptr);
    // }
    int y = 0;
    point_t p1WorldXY = {.x = context_ptr->origin.x - context_ptr->renderDistance, .y = y, .z = 0};
    point_t p2WorldXY = {.x = context_ptr->origin.x + context_ptr->renderDistance, .y = y, .z = 0};
    drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, (y == 0) ? &red : &gray, context_ptr);
    for(int x = context_ptr->origin.x - context_ptr->renderDistance; x <= context_ptr->origin.x + context_ptr->renderDistance; x++){
        // XZ plan
        point_t p1WorldXZ = {.x = x, .y = 0, .z = context_ptr->origin.z - context_ptr->renderDistance};
        point_t p2WorldXZ = {.x = x, .y = 0, .z = context_ptr->origin.z + context_ptr->renderDistance};
        drawGrid(&p1WorldXZ, &p2WorldXZ, &translateVector, (x == 0) ? &green : &gray, context_ptr);
        // // XY plan
        // point_t p1WorldXY = {.x = x, .y = context_ptr->origin.y - context_ptr->renderDistance, .z = 0};
        // point_t p2WorldXY = {.x = x, .y = context_ptr->origin.y + context_ptr->renderDistance, .z = 0};
        // drawGrid(&p1WorldXY, &p2WorldXY, &translateVector, &black, context_ptr);
    }
    
    return EXIT_SUCCESS;
}