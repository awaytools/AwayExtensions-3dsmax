#include "maxawdexporter.h"

void MaxAWDExporter::ReadAWDEffectMethods(Modifier *node_mod, INode* originalNode){
    int num_params = node_mod->NumParamBlocks();
    int cntBlocks=0;
    IParamBlock2* pb ;
    char * settingsNodeID_ptr = NULL;
    bool simpleMode=true;
    int animType=0;
    int p=0;
    bool autoAssignMethod=false;
    bool isColorMatrix=false;
    bool isColorTransform=false;
    bool isEnvMap=false;
    bool isLightMapMethod=false;
    bool isProjectiveTextureMethod=false;
    bool isRimLightMethod=false;
    bool isAlphaMaskMethod=false;
    bool isRefractionEnvMapMethod=false;
    bool isOutlineMethod=false;
    bool isFresnelEnvMapMethod=false;
    bool isFogMethod=false;
    int idxColorMatrix=1;
    int idxColorTransform=2;
    int idxEnvMap=3;
    int idxLightMapMethod=4;
    int idxProjectiveTextureMethod=5;
    int idxRimLightMethod=6;
    int idxAlphaMaskMethod=7;
    int idxRefractionEnvMapMethod=8;
    int idxOutlineMethod=9;
    int idxFresnelEnvMapMethod=10;
    int idxFogMethod=11;
    pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 11);
    if(pb!=NULL){
        int numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);
            ParamType2 paramtype = pb->GetParameterType(pid);
            char * paramName=W2A(def.int_name);
            if (paramtype==TYPE_BOOL){
                if (ATTREQ(paramName, "autoAssignMethod"))
                    autoAssignMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isColorMatrix"))
                    isColorMatrix = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isColorTransform"))
                    isColorTransform = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isEnvMap"))
                    isEnvMap = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isLightMapMethod"))
                    isLightMapMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isProjectiveTextureMethod"))
                    isProjectiveTextureMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isRimLightMethod"))
                    isRimLightMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isAlphaMaskMethod"))
                    isAlphaMaskMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isRefractionEnvMapMethod"))
                    isRefractionEnvMapMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isOutlineMethod"))
                    isOutlineMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isFresnelEnvMapMethod"))
                    isFresnelEnvMapMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isFogMethod"))
                    isFogMethod = (0 != pb->GetInt(pid));
            }
            if (paramtype==TYPE_INT){
                if (ATTREQ(paramName, "idxColorMatrix"))
                    idxColorMatrix = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxColorTransform"))
                    idxColorTransform = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxEnvMap"))
                    idxEnvMap = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxLightMapMethod"))
                    idxLightMapMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxProjectiveTextureMethod"))
                    idxProjectiveTextureMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxRimLightMethod"))
                    idxRimLightMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxAlphaMaskMethod"))
                    idxAlphaMaskMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxRefractionEnvMapMethod"))
                    idxRefractionEnvMapMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxOutlineMethod"))
                    idxOutlineMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxFresnelEnvMapMethod"))
                    idxFresnelEnvMapMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxFogMethod"))
                    idxFogMethod = pb->GetInt(pid);
            }
            if (paramtype==TYPE_STRING){
                if (ATTREQ(paramName, "thisAWDID"))
                    settingsNodeID_ptr = W2A(pb->GetStr(pid));
            }
            free(paramName);
        }
    }
    else{
        return;
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    AWDBlockList * awdEffectBlocks=(AWDBlockList *)methodsCache->Get(settingsNodeID_ptr);
    if (awdEffectBlocks==NULL){
        IntCache * orderEffectMethodsCache=new IntCache();
        if (isColorMatrix){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+13);
            strcpy(thisName, bname);
            strcat(thisName, "_colormatrix");
            thisName[strlen(bname)+12]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_COLORMATRIX);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxColorMatrix);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                AWD_field_ptr colorMatrix_val;
                colorMatrix_val.v = malloc(sizeof(awd_float64)*20);
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"colorMatrixVal1"))
                            colorMatrix_val.f64[0]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal2"))
                            colorMatrix_val.f64[1]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal3"))
                            colorMatrix_val.f64[2]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal4"))
                            colorMatrix_val.f64[3]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal5"))
                            colorMatrix_val.f64[4]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal6"))
                            colorMatrix_val.f64[5]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal7"))
                            colorMatrix_val.f64[6]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal8"))
                            colorMatrix_val.f64[7]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal9"))
                            colorMatrix_val.f64[8]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal10"))
                            colorMatrix_val.f64[9]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal11"))
                            colorMatrix_val.f64[10]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal12"))
                            colorMatrix_val.f64[11]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal13"))
                            colorMatrix_val.f64[12]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal14"))
                            colorMatrix_val.f64[13]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal15"))
                            colorMatrix_val.f64[14]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal16"))
                            colorMatrix_val.f64[15]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal17"))
                            colorMatrix_val.f64[16]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal18"))
                            colorMatrix_val.f64[17]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal19"))
                            colorMatrix_val.f64[18]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal20"))
                            colorMatrix_val.f64[19]=pb->GetFloat(pid);
                    }
                    free(paramName_ptr);
                }
                //TODO: check if the matrix is default...
                awdFXMethod->get_effect_props()->set(PROPS_NUMBER1, colorMatrix_val, sizeof(awd_float64)*20, AWD_FIELD_FLOAT64);
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isColorTransform){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+16);
            strcpy(thisName, bname);
            strcat(thisName, "_colortransform");
            thisName[strlen(bname)+15]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_COLORTRANSFORM);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxColorTransform);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 1);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                Point3 color;
                bool hasColor=false;
                double alpha=0;
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_RGBA){
                        if (ATTREQ(paramName_ptr,"colorTransformColor")){
                            color = pb->GetColor(pid);
                            hasColor=true;
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"colorTransformVal1"))
                            alpha = pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorTransformVal2"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER2, pb->GetFloat(pid), 1.0);
                        if (ATTREQ(paramName_ptr,"colorTransformVal3"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER3, pb->GetFloat(pid), 1.0);
                        if (ATTREQ(paramName_ptr,"colorTransformVal4"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER4, pb->GetFloat(pid), 1.0);
                        if (ATTREQ(paramName_ptr,"colorTransformVal5"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER1, pb->GetFloat(pid), 1.0);
                    }
                    free(paramName_ptr);
                }
                if (hasColor)
                    awdFXMethod->add_color_method_prop(PROPS_COLOR1, createARGB(alpha*255, color.x*255, color.y*255, color.z*255), 0xffffff);
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isEnvMap){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+8);
            strcpy(thisName, bname);
            strcat(thisName, "_envmap");
            thisName[strlen(bname)+7]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_ENVMAP);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxEnvMap);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 2);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"envMapAlpha"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER1, pb->GetFloat(pid), 1.0);
                    }
                    if (paramtype==TYPE_TEXMAP){
                        if (ATTREQ(paramName_ptr,"envMapMask")){
                            Texmap *tex = pb->GetTexmap(pid);
                            AWDBitmapTexture * newtex=NULL;
                            if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE, FXENVMAPMASK);
                                if ((newtex!=NULL)&&(newtex->get_isValid()))
                                    awdFXMethod->set_awdBlock2(newtex);
                            }
                        }
                    }
                    if (paramtype==TYPE_REFTARG){
                        if (ATTREQ(paramName_ptr,"envMap")){
                            //TODO: THERE IS NO OPTION TO ADD THE ENVMAP IN THE INTERFACE YET, so this is not tested:
                            Mtl *thisMtl = (Mtl *)pb->GetReferenceTarget(pid);
                            if (thisMtl!=NULL){
                                MSTR matClassName;
                                thisMtl->GetClassName(matClassName);
                                char * matClassName_ptr=W2A(matClassName);
                                if (ATTREQ(matClassName_ptr,"AWDCubeMaterial")){
                                    AWDCubeTexture* newCubetex = ExportAWDCubeTexure((MultiMtl *)thisMtl);
                                    if (newCubetex!=NULL){
                                        awdFXMethod->set_awdBlock1(newCubetex);
                                    }
                                }
                                free(matClassName_ptr);
                            }
                        }
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isLightMapMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+10);
            strcpy(thisName, bname);
            strcat(thisName, "_lightmap");
            thisName[strlen(bname)+9]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_LIGHTMAP);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxLightMapMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 3);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_TEXMAP){
                        if (ATTREQ(paramName_ptr,"thislightMap") ){
                            Texmap *tex = pb->GetTexmap(pid);
                            AWDBitmapTexture * newtex=NULL;
                            if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE, FXLIGHTMAP);
                                if ((newtex!=NULL)&&(newtex->get_isValid()))
                                    awdFXMethod->set_awdBlock1(newtex);
                            }
                        }
                    }
                    if (paramtype==TYPE_BOOL){
                        if (ATTREQ(paramName_ptr,"useSecUV") )
                            awdFXMethod->add_bool_method_prop(PROPS_BOOL1, (0 != pb->GetInt(pid)), false);
                    }
                    if (paramtype==TYPE_INT){
                        if (ATTREQ(paramName_ptr,"blendMode") ){
                            AWD_field_ptr lightMapBlendMode_val;
                            //TODO: translate to correct blendmode int for AWD
                            int blendMode=10;
                            if(pb->GetInt(pid)==2)
                                blendMode=1;
                            lightMapBlendMode_val.v = malloc(sizeof(awd_uint8));
                            *lightMapBlendMode_val.ui8 = (awd_uint8)blendMode;
                            awdFXMethod->get_effect_props()->set(PROPS_INT8_1, lightMapBlendMode_val, sizeof(awd_uint8), AWD_FIELD_UINT8);
                        }
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isProjectiveTextureMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+19);
            strcpy(thisName, bname);
            strcat(thisName, "_projectivetexture");
            thisName[strlen(bname)+18]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_PROJECTIVE_TEXTURE);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxProjectiveTextureMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 4);
            char * texProjectAWDID_ptr=NULL;
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_STRING){
                        if (ATTREQ(paramName_ptr,"pt_projectorID")){
                            texProjectAWDID_ptr=W2A(pb->GetStr(pid));
                            AWDTextureProjector* texProjector = (AWDTextureProjector*)textureProjectorCache->Get(texProjectAWDID_ptr);
                            if (texProjector!=NULL){
                                awdFXMethod->set_awdBlock1(texProjector);
                            }
                        }
                    }
                    if (paramtype==TYPE_INT){
                        if (ATTREQ(paramName_ptr,"pt_blendMode") ){
                            AWD_field_ptr lightMapBlendMode_val;
                            //TODO: translate to correct blendmode int for AWD
                            int blendMode=10;
                            if(pb->GetInt(pid)==2)
                                blendMode=16;
                            if(pb->GetInt(pid)==3)
                                blendMode=17;
                            lightMapBlendMode_val.v = malloc(sizeof(awd_uint8));
                            *lightMapBlendMode_val.ui8 = (awd_uint8)blendMode;
                            awdFXMethod->get_effect_props()->set(PROPS_INT8_1, lightMapBlendMode_val, sizeof(awd_uint8), AWD_FIELD_UINT8);
                        }
                    }
                    free(paramName_ptr);
                    //TODO: READ PROJECTIVE TEXTURE METHOD (ITS DISABLED FOR NOW)
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
            if(texProjectAWDID_ptr!=NULL)
                free(texProjectAWDID_ptr);
        }
        if (isRimLightMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+10);
            strcpy(thisName, bname);
            strcat(thisName, "_rimlight");
            thisName[strlen(bname)+9]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_RIMLIGHT);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxRimLightMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 5);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_RGBA){
                        if (ATTREQ(paramName_ptr,"thisColor") ){
                            Point3 color = pb->GetColor(pid);
                            awdFXMethod->add_color_method_prop(PROPS_COLOR1, createARGB(255, color.x*255, color.y*255, color.z*255), 0xffffff);
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"thisStrength") )
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER1, pb->GetFloat(pid), 0.4);
                        if (ATTREQ(paramName_ptr,"thispower") )
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER2, pb->GetFloat(pid), 2.0);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isAlphaMaskMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+11);
            strcpy(thisName, bname);
            strcat(thisName, "_alphamask");
            thisName[strlen(bname)+10]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_ALPHA_MASK);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxAlphaMaskMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 6);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_TEXMAP){
                        if (ATTREQ(paramName_ptr,"mask_map") ){
                            Texmap *tex = pb->GetTexmap(pid);
                            AWDBitmapTexture * newtex=NULL;
                            if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE, FXMASK);
                                if ((newtex!=NULL)&&(newtex->get_isValid()))
                                    awdFXMethod->set_awdBlock1(newtex);
                            }
                        }
                    }
                    if (paramtype==TYPE_BOOL){
                        if (ATTREQ(paramName_ptr,"mask_useSecUV"))
                            awdFXMethod->add_bool_method_prop(PROPS_BOOL1, (0 != pb->GetInt(pid)), false);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isRefractionEnvMapMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+18);
            strcpy(thisName, bname);
            strcat(thisName, "_refractionenvmap");
            thisName[strlen(bname)+17]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_REFRACTION_ENVMAP);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxRefractionEnvMapMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 7);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"refract_index"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER1, pb->GetFloat(pid), 0.1);
                        if (ATTREQ(paramName_ptr,"refract_dispersion1"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER2, pb->GetFloat(pid), 0.01);
                        if (ATTREQ(paramName_ptr,"refract_dispersion2"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER3, pb->GetFloat(pid), 0.01);
                        if (ATTREQ(paramName_ptr,"refract_dispersion3"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER4, pb->GetFloat(pid), 0.01);
                        if (ATTREQ(paramName_ptr,"refract_alpha"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER5, pb->GetFloat(pid), 1.0);
                    }
                    if (paramtype==TYPE_REFTARG){
                        if (ATTREQ(paramName_ptr,"refract_envMap")){
                            Mtl *thisMtl = (Mtl *)pb->GetReference(pid);
                            if (thisMtl!=NULL){
                                MSTR matClassName;
                                thisMtl->GetClassName(matClassName);
                                char * matClassName_ptr=W2A(matClassName);
                                if (ATTREQ(matClassName_ptr,"AWDCubeMaterial")){
                                    AWDCubeTexture* newCubetex = ExportAWDCubeTexure((MultiMtl *)thisMtl);
                                    if (newCubetex!=NULL){
                                        awdFXMethod->set_awdBlock1(newCubetex);
                                    }
                                }
                                free(matClassName_ptr);
                            }
                        }
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod - should never happn
            }
        }
        if (isOutlineMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+9);
            strcpy(thisName, bname);
            strcat(thisName, "_outline");
            thisName[strlen(bname)+8]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_OUTLINE);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxOutlineMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 8);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_RGBA){
                        if (ATTREQ(paramName_ptr,"outlineColor") ){
                            Point3 color = pb->GetColor(pid);
                            awdFXMethod->add_color_method_prop(PROPS_COLOR1, createARGB(255, color.x*255, color.y*255, color.z*255), 0x000000);
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"outlineSize") )
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER1, pb->GetFloat(pid), 1.0);
                    }
                    if (paramtype==TYPE_BOOL){
                        if (ATTREQ(paramName_ptr,"innerLines") )
                            awdFXMethod->add_bool_method_prop(PROPS_BOOL1, (0 != pb->GetInt(pid)), true);
                        if (ATTREQ(paramName_ptr,"dedicadedMesh") )
                            awdFXMethod->add_bool_method_prop(PROPS_BOOL2, (0 != pb->GetInt(pid)), false);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isFresnelEnvMapMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+15);
            strcpy(thisName, bname);
            strcat(thisName, "_fresnelenvmap");
            thisName[strlen(bname)+14]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_FRESNEL_ENVMAP);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxFresnelEnvMapMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 9);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_REFTARG){
                        if (ATTREQ(paramName_ptr,"fresnel_EnvMap") ){
                            Mtl *thisMtl = (Mtl *)pb->GetReference(pid);
                            if (thisMtl!=NULL){
                                MSTR matClassName;
                                thisMtl->GetClassName(matClassName);
                                char * matClassName_ptr=W2A(matClassName);
                                if (ATTREQ(matClassName_ptr,"AWDCubeMaterial") ){
                                    AWDCubeTexture* newCubetex = ExportAWDCubeTexure((MultiMtl *)thisMtl);
                                    if (newCubetex!=NULL){
                                        awdFXMethod->set_awdBlock1(newCubetex);
                                    }
                                }
                                free(matClassName_ptr);
                            }
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"fresnel_alpha"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER1, pb->GetFloat(pid), 1.0);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isFogMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+5);
            strcpy(thisName, bname);
            strcat(thisName, "_fog");
            thisName[strlen(bname)+4]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_FOG);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxFogMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 10);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_RGBA){
                        if (ATTREQ(paramName_ptr,"fogColor")){
                            Point3 color = pb->GetColor(pid);
                            awdFXMethod->add_color_method_prop(PROPS_COLOR1, createARGB(255, color.x*255, color.y*255, color.z*255), 0x808080);
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"fogDistanceMin"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER1, pb->GetFloat(pid), 0);
                        if (ATTREQ(paramName_ptr,"fogDistanceMax"))
                            awdFXMethod->add_number_method_prop(PROPS_NUMBER2, pb->GetFloat(pid), 1000.0);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        awdEffectBlocks=orderEffectMethodsCache->GetKeysOrderedByVal();
        if(autoAssignMethod){
                AWDBlockList * awdAutoEffectBlocks=(AWDBlockList*)autoApplyMethodsToINodeCache->Get(originalNode);
                if(awdAutoEffectBlocks==NULL){
                    awdAutoEffectBlocks=new AWDBlockList();
                    autoApplyMethodsToINodeCache->Set(originalNode, awdAutoEffectBlocks);
                }
                AWDBlock * awdBlock=NULL;
                AWDBlockIterator *it;
                it = new AWDBlockIterator(awdEffectBlocks);
                while ((awdBlock = (AWDBlock*)it->next()) != NULL) {
                    awdAutoEffectBlocks->append(awdBlock);
                }
                delete it;
        }
        methodsCache->Set(settingsNodeID_ptr, awdEffectBlocks);
        delete orderEffectMethodsCache;
    }
    else{
        if(autoAssignMethod){
            AWDBlockList * awdAutoEffectBlocks=(AWDBlockList*)autoApplyMethodsToINodeCache->Get(originalNode);
            if(awdAutoEffectBlocks==NULL){
                awdAutoEffectBlocks=new AWDBlockList();
                autoApplyMethodsToINodeCache->Set(originalNode, awdAutoEffectBlocks);
            }
            AWDBlock * awdBlock=NULL;
            AWDBlockIterator *it;
            it = new AWDBlockIterator(awdEffectBlocks);
            while ((awdBlock = (AWDBlock*)it->next()) != NULL) {
                awdAutoEffectBlocks->append(awdBlock);
            }
            delete it;
        }
    }
    if (settingsNodeID_ptr!=NULL)
        free(settingsNodeID_ptr);
}