#include "maxawdexporter.h"

void MaxAWDExporter::GetCustomAWDMaterialSettings(StdMat *mtl, AWDMaterial * awdMat)
{
    ICustAttribContainer *attributes = mtl->GetCustAttribContainer();
    AWDShadingMethod * newEnvMapMethod=NULL;
    AWDShadingMethod * newBaseDiffuseMethod=NULL;
    AWDShadingMethod * newBaseSpecularMethod=NULL;
    AWDShadingMethod * newDiffuseMethod=NULL;
    AWDShadingMethod * newSpecularMethod=NULL;
    AWDShadingMethod * newNormalMethod=NULL;
    char * shadowMethod_ptr = NULL;
    if (attributes) {
        int a=0;
        int p=0;
        int numAttribs=0;
        numAttribs = attributes->GetNumCustAttribs();
        for (a=0; a<numAttribs; a++) {
            int t=0;
            bool useShading=false;
            bool useMethods=true;//set to true, because it wasnt in earlier version, so mat-settings from earlier version would export without methods...
            CustAttrib *attr = attributes->GetCustAttrib(a);
            for (t=0; t<attr->NumParamBlocks(); t++) {
                IParamBlock2 *block = attr->GetParamBlock(t);
                char * localName_ptr=W2A(block->GetLocalName());
                if (ATTREQ(localName_ptr,"AWD_MaterialSettingsparams") ){
                    bool multiPassMatenabled=false;
                    bool multiPassMat=false;
                    bool repeatMatenabled=false;
                    bool repeatMat=false;
                    bool smoothMatenabled=false;
                    bool smoothMat=false;
                    bool mipmapMatenabled=false;
                    bool mipmapMat=false;
                    bool alphaBlendingMatenabled=false;
                    bool alphaBlendingMat=false;
                    bool alphaTresholdMatenabled=false;
                    float alphaTresholdMat=0;
                    bool premultipliedMatenabled=false;
                    bool premultipliedMat=false;
                    bool blendModeenabled=false;
                    int blendMode=0;
                    bool MainUVenabled=false;
                    int mainUVChannel=0;
                    bool SecUVenabled=false;
                    int secUVChannel=0;
                    bool AmbientLevelenabled=false;
                    float ambientLevel=0;
                    bool SpecularLevelenabled=false;
                    float specularLevel=0;
                    bool GlossLevelenabled=false;
                    int glossLevel=0;
                    for (p=0; p<block->NumParams(); p++) {
                        ParamID pid = block->IndextoID(p);
                        ParamDef def = block->GetParamDef(pid);
                        ParamType2 typeID=block->GetParameterType(pid);
                        // TO DO: READ IN ALL THE MATERIAL PROPERTIES
                        char * paramName_ptr=W2A(def.int_name);
                        if (block->GetParameterType(pid)==TYPE_BOOL){
                            if (ATTREQ(paramName_ptr,"methodenabled"))
                                useMethods=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"shadingenabled"))
                                useShading=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"multiPassMatenabled"))
                                multiPassMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"multiPassMat"))
                                multiPassMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"repeatMatenabled"))
                                repeatMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"repeatMat"))
                                repeatMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"smoothMatenabled"))
                                smoothMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"smoothMat"))
                                smoothMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"mipmapMatenabled"))
                                mipmapMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"mipmapMat"))
                                mipmapMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"alphaBlendingMatenabled"))
                                alphaBlendingMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"alphaBlendingMat"))
                                alphaBlendingMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"alphaTresholdMatenabled"))
                                alphaTresholdMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"premultipliedMatenabled"))
                                premultipliedMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"premultipliedMat"))
                                premultipliedMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"blendModeenabled"))
                                blendModeenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"MainUVenabled"))
                                MainUVenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"SecUVenabled"))
                                SecUVenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"AmbientLevelenabled"))
                                AmbientLevelenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"SpecularLevelenabled"))
                                SpecularLevelenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"GlossLevelenabled"))
                                GlossLevelenabled=(0 != block->GetInt(pid));
                        }
                        if (block->GetParameterType(pid)==TYPE_FLOAT){
                            if (ATTREQ(paramName_ptr,"alphaTresholdMat"))
                                alphaTresholdMat= block->GetFloat(pid);
                            if (ATTREQ(paramName_ptr,"ambientLevel"))
                                ambientLevel= block->GetFloat(pid);
                            if (ATTREQ(paramName_ptr,"specularLevel"))
                                specularLevel= block->GetFloat(pid);
                        }
                        if (block->GetParameterType(pid)==TYPE_INT){
                            if (ATTREQ(paramName_ptr,"blendMode"))
                                blendMode= block->GetInt(pid);
                            if (ATTREQ(paramName_ptr,"mainUVChannel"))
                                mainUVChannel= block->GetInt(pid);
                            if (ATTREQ(paramName_ptr,"secUVChannel"))
                                secUVChannel= block->GetInt(pid);
                            if (ATTREQ(paramName_ptr,"glossLevel"))
                                glossLevel= block->GetInt(pid);
                        }
                        if (block->GetParameterType(pid)==TYPE_STRING){
                            if (ATTREQ(paramName_ptr, "shadowModID"))
                                shadowMethod_ptr= W2A(block->GetStr(pid));
                        }
                        free(paramName_ptr);
                    }
                    if((shadowMethod_ptr!=NULL)&&(opts->IncludeShadows())){
                        AWDBlock * shadowMethod=(AWDBlock *)shadowMethodsCache->Get(shadowMethod_ptr);
                        if(shadowMethod!=NULL)
                            awdMat->set_shadowMethod(shadowMethod);
                    }
                    if(shadowMethod_ptr!=NULL)
                        free(shadowMethod_ptr);
                    if(multiPassMatenabled)
                        awdMat->set_multiPass(multiPassMat);
                    if(repeatMatenabled)
                        awdMat->set_repeat(repeatMat);
                    if(smoothMatenabled)
                        awdMat->set_smooth(smoothMat);
                    if(mipmapMatenabled)
                        awdMat->set_mipmap(mipmapMat);
                    if(alphaBlendingMatenabled)
                        awdMat->set_alpha_blending(alphaBlendingMat);
                    if(alphaTresholdMatenabled)
                        awdMat->set_alpha_threshold(alphaTresholdMat);
                    if(premultipliedMatenabled)
                        awdMat->set_premultiplied(premultipliedMat);
                    if(blendModeenabled){
                        if(blendMode==1)awdMat->set_blendMode(0);//Normal
                        else if(blendMode==2)awdMat->set_blendMode(8);//Layer
                        else if(blendMode==3)awdMat->set_blendMode(10);//Multiply
                        else if(blendMode==4)awdMat->set_blendMode(1);//ADD
                        else if(blendMode==5)awdMat->set_blendMode(2);//Alpha
                    }
                    if(MainUVenabled)
                        awdMat->set_mappingChannel(mainUVChannel);
                    if(SecUVenabled)
                        awdMat->set_secondMappingChannel(secUVChannel);
                    if(AmbientLevelenabled)
                        awdMat->set_ambientStrength(ambientLevel);
                    if(SpecularLevelenabled)
                        awdMat->set_specularStrength(specularLevel);
                    if(GlossLevelenabled)
                        awdMat->set_glossStrength(glossLevel);
                }
                free(localName_ptr);
            }
            if(!opts->IncludeShadings())
                useShading=false;
            for (t=0; t<attr->NumParamBlocks(); t++) {
                IParamBlock2 *block = attr->GetParamBlock(t);
                char * localName_ptr=W2A(block->GetLocalName());
                if ((ATTREQ(localName_ptr,"AWD_EffectMethodsparams"))&&(opts->IncludeMethods()&&(useMethods))){
                    for (p=0; p<block->NumParams(); p++) {
                        ParamID pid = block->IndextoID(p);
                        ParamDef def = block->GetParamDef(pid);
                        ParamType2 typeID=block->GetParameterType(pid);
                        char * paramName_ptr=W2A(def.int_name);
                        if (ATTREQ(paramName_ptr,"saveMethodIds") ){
                            int cntp=0;
                            AWDBlockList * newEffectList= new AWDBlockList();
                            for (cntp=0;cntp<block->Count(pid);cntp++){
                                char * effectlistID=W2A(block->GetStr(pid, 0, cntp));
                                AWDBlockList * foundEffectList=(AWDBlockList *)methodsCache->Get(effectlistID);
                                if (foundEffectList!=NULL){
                                    AWDBlock * awdBlock=NULL;
                                    AWDBlockIterator *it;
                                    it = new AWDBlockIterator(foundEffectList);
                                    while ((awdBlock = (AWDBlock*)it->next()) != NULL) {
                                        newEffectList->append(awdBlock);
                                    }
                                    delete it;
                                }
                                free(effectlistID);
                            }
                            awdMat->set_effectMethods(newEffectList);
                        }
                        free(paramName_ptr);
                    }
                }
                else if (ATTREQ(localName_ptr,"AWDShadingParams") ){
                    if (useShading){
                        awdMat->resetShadingMethods();
                        for (p=0; p<block->NumParams(); p++) {
                            ParamID pid = block->IndextoID(p);
                            ParamDef def = block->GetParamDef(pid);
                            ParamType2 typeID=block->GetParameterType(pid);
                            char * paramName_ptr=W2A(def.int_name);
                            if (block->GetParameterType(pid)==TYPE_INT){
                                if (ATTREQ(paramName_ptr,"ambientMethod") ){
                                    int ambientMethod=block->GetInt(pid);
                                    if (ambientMethod==2){
                                        newEnvMapMethod=new AWDShadingMethod(AWD_SHADEMETHOD_ENV_AMBIENT);
                                    }
                                }
                                if (ATTREQ(paramName_ptr,"diffuseMethod") ){
                                    int diffuseMethod=block->GetInt(pid);
                                    if (diffuseMethod==2)
                                        newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_GRADIENT);
                                    else if (diffuseMethod==3)
                                        newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_WRAP);
                                    else if (diffuseMethod==4)
                                        newDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_LIGHTMAP);
                                    else if (diffuseMethod==5)
                                        newDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_CELL);
                                }
                                if (ATTREQ(paramName_ptr,"specMethod") ){
                                    int specularMethod=block->GetInt(pid);
                                    if (specularMethod==2)
                                        newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC);
                                    if (specularMethod==3)
                                        newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_PHONG);
                                    if (specularMethod==4)
                                        newSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_CELL);
                                    if (specularMethod==5)
                                        newSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_FRESNEL);
                                }
                                if (ATTREQ(paramName_ptr,"normalMethod") ){
                                    int normalMethod=block->GetInt(pid);
                                    if (normalMethod==2){
                                        newNormalMethod=new AWDShadingMethod(AWD_SHADEMETHOD_NORMAL_SIMPLE_WATER);
                                    }
                                }
                            }
                            free(paramName_ptr);
                        }
                    }
                }
                free(localName_ptr);
            }
            if (useShading){
                if (newDiffuseMethod!=NULL || newSpecularMethod!=NULL){
                    for (t=0; t<attr->NumParamBlocks(); t++) {
                        IParamBlock2 *block = attr->GetParamBlock(t);
                        char * localName_ptr=W2A(block->GetLocalName());
                        if (ATTREQ(localName_ptr,"LightMapDiffuseParams") ){
                            if (newDiffuseMethod!=NULL && newDiffuseMethod->get_shading_type()==AWD_SHADEMETHOD_DIFFUSE_LIGHTMAP){
                                for (p=0; p<block->NumParams(); p++) {
                                    ParamID pid = block->IndextoID(p);
                                    ParamDef def = block->GetParamDef(pid);
                                    ParamType2 typeID=block->GetParameterType(pid);
                                    char * paramName_ptr=W2A(def.int_name);
                                    if (block->GetParameterType(pid)==TYPE_INT){
                                        if (ATTREQ(paramName_ptr,"lightMap_baseMethod") ){
                                            int baseMethod=block->GetInt(pid);
                                            if (baseMethod==2)
                                                newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_GRADIENT);
                                            if (baseMethod==3)
                                                newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_WRAP);
                                        }
                                        if (ATTREQ(paramName_ptr,"light_map_blendMode") ){
                                            int blendMode=block->GetInt(pid);//TODO take care that the blendmode is correct
                                            newDiffuseMethod->add_int8_property(PROPS_INT8_1, blendMode, 10);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_TEXMAP){
                                        if (ATTREQ(paramName_ptr,"light_map") ){
                                            Texmap *tex = block->GetTexmap(pid);
                                            AWDBitmapTexture * newtex=NULL;
                                            if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                                newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE, SHADINGLIGHTMAP);
                                                if (newtex!=NULL)
                                                    newDiffuseMethod->set_awdBlock1(newtex);
                                            }
                                        }
                                    }
                                    free(paramName_ptr);
                                }
                            }
                        }
                        if (ATTREQ(localName_ptr,"CellDiffuseParams") ){
                            if (newDiffuseMethod!=NULL && newDiffuseMethod->get_shading_type()==AWD_SHADEMETHOD_DIFFUSE_CELL){
                                for (p=0; p<block->NumParams(); p++) {
                                    ParamID pid = block->IndextoID(p);
                                    ParamDef def = block->GetParamDef(pid);
                                    ParamType2 typeID=block->GetParameterType(pid);
                                    char * paramName_ptr=W2A(def.int_name);
                                    if (block->GetParameterType(pid)==TYPE_FLOAT){
                                        if (ATTREQ(paramName_ptr,"celldiffuse_smooth") ){
                                            double smooth=block->GetFloat(pid);//TODO take care that the blendmode is correct
                                            newDiffuseMethod->add_number_property(PROPS_NUMBER1, smooth, 0.1);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_INT){
                                        if (ATTREQ(paramName_ptr,"celldiffuse_level") ){
                                            int level=block->GetInt(pid);
                                            newDiffuseMethod->add_int8_property(PROPS_INT8_1, level, 3);
                                        }
                                        if (ATTREQ(paramName_ptr,"celldiffuse_baseMethod") ){
                                            int baseMethod=block->GetInt(pid);
                                            if (baseMethod==2)
                                                newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_GRADIENT);
                                            if (baseMethod==3)
                                                newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_WRAP);
                                        }
                                    }
                                    free(paramName_ptr);
                                }
                            }
                        }
                        if (ATTREQ(localName_ptr,"CellSpecularParams") ){
                            if (newSpecularMethod!=NULL && newSpecularMethod->get_shading_type()==AWD_SHADEMETHOD_SPECULAR_CELL){
                                for (p=0; p<block->NumParams(); p++) {
                                    ParamID pid = block->IndextoID(p);
                                    ParamDef def = block->GetParamDef(pid);
                                    ParamType2 typeID=block->GetParameterType(pid);
                                    char * paramName_ptr=W2A(def.int_name);
                                    if (block->GetParameterType(pid)==TYPE_FLOAT){
                                        if (ATTREQ(paramName_ptr,"cellspec_smooth") ){
                                            double smooth=block->GetFloat(pid);
                                            newSpecularMethod->add_number_property(PROPS_NUMBER2, smooth, 0.1);
                                        }
                                        if (ATTREQ(paramName_ptr,"cellspec_cutoff") ){
                                            double cutOff=block->GetFloat(pid);
                                            newSpecularMethod->add_number_property(PROPS_NUMBER1, cutOff, 0.5);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_INT){
                                        if (ATTREQ(paramName_ptr,"cellspec_baseMethod") ){
                                            int baseMethod=block->GetInt(pid);
                                            if (baseMethod==2)
                                                newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC);
                                            if (baseMethod==3)
                                                newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_PHONG);
                                        }
                                    }
                                    free(paramName_ptr);
                                }
                            }
                        }
                        if (ATTREQ(localName_ptr,"FresnelSpecularParams") ){
                            if (newSpecularMethod!=NULL && newSpecularMethod->get_shading_type()==AWD_SHADEMETHOD_SPECULAR_FRESNEL){
                                for (p=0; p<block->NumParams(); p++) {
                                    ParamID pid = block->IndextoID(p);
                                    ParamDef def = block->GetParamDef(pid);
                                    ParamType2 typeID=block->GetParameterType(pid);
                                    char * paramName_ptr=W2A(def.int_name);
                                    if (block->GetParameterType(pid)==TYPE_FLOAT){
                                        if (ATTREQ(paramName_ptr,"fresnelspec_reflect") ){
                                            double reflect=block->GetFloat(pid);
                                            newSpecularMethod->add_number_property(PROPS_NUMBER2, reflect, 0.1);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_BOOL){
                                        if (ATTREQ(paramName_ptr,"fresnelspec_basedOnSurface") ){
                                            bool basedOnSurface = (0 != block->GetInt(pid));
                                            newSpecularMethod->add_bool_property(PROPS_BOOL1, basedOnSurface, true);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_INT){
                                        if (ATTREQ(paramName_ptr,"fresnelspec_power") ){
                                            int power=block->GetInt(pid);
                                            newSpecularMethod->add_int_property(PROPS_INT1, power, 5);
                                        }
                                        if (ATTREQ(paramName_ptr,"fresnelspec_baseMethod") ){
                                            int baseMethod=block->GetInt(pid);
                                            if (baseMethod==2)
                                                newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC);
                                            if (baseMethod==3)
                                                newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_PHONG);
                                        }
                                    }
                                    free(paramName_ptr);
                                }
                            }
                        }
                        free(localName_ptr);
                    }
                }
                for (t=0; t<attr->NumParamBlocks(); t++) {
                    int p=0;
                    IParamBlock2 *block = attr->GetParamBlock(t);
                    char * localName_ptr=W2A(block->GetLocalName());
                    if (ATTREQ(localName_ptr,"EnvMapAmbientParams") ){
                        if (newEnvMapMethod!=NULL){
                            for (p=0; p<block->NumParams(); p++) {
                                ParamID pid = block->IndextoID(p);
                                ParamDef def = block->GetParamDef(pid);
                                ParamType2 typeID=block->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (typeID==TYPE_REFTARG){
                                    if (ATTREQ(paramName_ptr,"ambient_envMap")){
                                        //TODO: THERE IS NO OPTION TO ADD THE ENVMAP IN THE INTERFACE YET, so this is not tested:
                                        Mtl *thisMtl = (Mtl *)block->GetReferenceTarget(pid);
                                        if (thisMtl!=NULL){
                                            MSTR matClassName;
                                            thisMtl->GetClassName(matClassName);
                                            char * matClassName_ptr=W2A(matClassName);
                                            if (ATTREQ(matClassName_ptr,"AWDCubeMaterial")){
                                                AWDCubeTexture* newCubetex = ExportAWDCubeTexure((MultiMtl *)thisMtl);
                                                if (newCubetex!=NULL){
                                                    newEnvMapMethod->set_awdBlock1(newCubetex);
                                                }
                                            }
                                            free(matClassName_ptr);
                                        }
                                    }
                                }
                                free(paramName_ptr);
                            }
                        }
                    }
                    else if (ATTREQ(localName_ptr,"GradientDiffuseParams") ){
                        if (newBaseDiffuseMethod!=NULL && newBaseDiffuseMethod->get_shading_type()==AWD_SHADEMETHOD_DIFFUSE_GRADIENT){
                            for (p=0; p<block->NumParams(); p++) {
                                ParamID pid = block->IndextoID(p);
                                ParamDef def = block->GetParamDef(pid);
                                ParamType2 typeID=block->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (ATTREQ(paramName_ptr,"gradientdiffuse_map") ){
                                    if (block->GetParameterType(pid)==TYPE_TEXMAP){
                                        Texmap *tex = block->GetTexmap(pid);
                                        AWDBitmapTexture * newtex=NULL;
                                        if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                            newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE, SHADINGGRADIENTDIFFUSE);
                                            if (newtex!=NULL)
                                                newBaseDiffuseMethod->set_awdBlock1(newtex);
                                        }
                                    }
                                }
                                free(paramName_ptr);
                            }
                        }
                    }
                    else if (ATTREQ(localName_ptr,"WarpDiffuseParams") ){
                        if (newBaseDiffuseMethod!=NULL && newBaseDiffuseMethod->get_shading_type()==AWD_SHADEMETHOD_DIFFUSE_WRAP){
                            for (p=0; p<block->NumParams(); p++) {
                                ParamID pid = block->IndextoID(p);
                                ParamDef def = block->GetParamDef(pid);
                                ParamType2 typeID=block->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (block->GetParameterType(pid)==TYPE_FLOAT){
                                    if (ATTREQ(paramName_ptr,"warpdiffuse_factor") ){
                                        double factor=block->GetFloat(pid);
                                        newBaseDiffuseMethod->add_number_property(PROPS_NUMBER1, factor, 5);
                                    }
                                }
                                free(paramName_ptr);
                            }
                        }
                    }
                    else if (ATTREQ(localName_ptr,"SimpleWaterNormalParams") ){
                        if (newNormalMethod!=NULL){
                            for (p=0; p<block->NumParams(); p++) {
                                ParamID pid = block->IndextoID(p);
                                ParamDef def = block->GetParamDef(pid);
                                ParamType2 typeID=block->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (ATTREQ(paramName_ptr,"waternormal_map") ){
                                    if (block->GetParameterType(pid)==TYPE_TEXMAP){
                                        Texmap *tex = block->GetTexmap(pid);
                                        AWDBitmapTexture * newtex=NULL;
                                        if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                            newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE, SHADINGSIMPLEWATERNORMAL);
                                            if (newtex!=NULL)
                                                newNormalMethod->set_awdBlock1(newtex);
                                        }
                                    }
                                }
                                free(paramName_ptr);
                            }
                        }
                    }
                    free(localName_ptr);
                }
                if(newEnvMapMethod!=NULL)
                    awdMat->add_method(newEnvMapMethod);

                if(newBaseDiffuseMethod!=NULL)
                    awdMat->add_method(newBaseDiffuseMethod);
                if(newDiffuseMethod!=NULL)
                    awdMat->add_method(newDiffuseMethod);

                if(newBaseSpecularMethod!=NULL)
                    awdMat->add_method(newBaseSpecularMethod);
                if(newSpecularMethod!=NULL)
                    awdMat->add_method(newSpecularMethod);

                if(newNormalMethod!=NULL)
                    awdMat->add_method(newNormalMethod);
            }
        }
    }
}

AWDBlock *MaxAWDExporter::getColorMatForObject(INode *node, boolean black){
    awd_color color = NULL;
    if (!black){
        color = convertColor(node->GetWireColor());    }
    else{
        color=convertColor((DWORD)0x000000 );
    }
    // Look in the cache for an existing "default" color material
    // that matches the color of this object. If none exists,
    // create a new one and store it in the cache.
    AWDMaterial *awdMtl;
    awdMtl = colMtlCache->Get(color);
    if (awdMtl == NULL) {
        char * matName_ptr = W2A(node->GetName());
        char * thisName = (char*)malloc(strlen(matName_ptr)+10);
        strcpy(thisName, matName_ptr);
        strcat(thisName, "_colorMat");
        thisName[strlen(matName_ptr)+9]=0;
        free(matName_ptr);
        awdMtl = new AWDMaterial(thisName, strlen(thisName));
        free(thisName);
        awdMtl->set_type(AWD_MATTYPE_COLOR);
        awdMtl->set_color(color);
        colMtlCache->Set(color, awdMtl);
        awd->add_material(awdMtl);
    }
    return awdMtl;
}

AWDBlockList *MaxAWDExporter::GetMaterialsForMeshInstance(INode *node)
{
    //output_debug_string("         -->Get Materials For MeshInstance");
    AWDBlockList * awdBlockList= new AWDBlockList();
    Mtl *mtl = node->GetMtl();
    if (mtl == NULL) {
        awdBlockList->append(getColorMatForObject(node, false));
        return awdBlockList;
    }
    else {
        int i=0;
        if (mtl->IsSubClassOf(Class_ID(DMTL_CLASS_ID, 0))) {
            StdMat *stdMtl = (StdMat *)mtl;
            AWDMaterial *awdmat;
            awdmat = ExportOneMaterial(stdMtl);
            awdBlockList->force_append(awdmat);
            return awdBlockList;
        }
        else if (mtl->IsSubClassOf(Class_ID(MULTI_CLASS_ID, 0))) {
            MultiMtl *multiMtl = (MultiMtl *)mtl;
            int numMaterials=multiMtl->NumSubMtls();
            for (i=0; i<numMaterials; i++) {
                Mtl* new_material=multiMtl->GetSubMtl(i);
                bool matExported=false;
                if (new_material!=NULL){
                    if (new_material->IsSubClassOf(Class_ID(DMTL_CLASS_ID, 0))) {
                        StdMat *stdMtl = (StdMat *)new_material;
                        AWDMaterial *awdmat;
                        awdmat = ExportOneMaterial(stdMtl);
                        awdBlockList->force_append(awdmat);
                        matExported=true;
                    }
                }
                if (!matExported){
                    awdBlockList->force_append(getColorMatForObject(node, true));
                }
            }
        }
        else{
            // not a multimaterial or standart material -> create a color-material with object-color
            AWDMaterial *awdmat=(AWDMaterial*)getColorMatForObject(node, false);
            awdBlockList->append(awdmat);
        }
    }
    if (awdBlockList->get_num_blocks()==0)
        awdBlockList->append(getColorMatForObject(node, false));
    return awdBlockList;
}

AWDMaterial *MaxAWDExporter::ExportOneMaterial(StdMat *mtl)
{
    //output_debug_string("         -->ExportOneMaterial");
    AWDMaterial *awdMtl=NULL;
    if (mtl != NULL) {
        awdMtl = (AWDMaterial *)materialCache->Get(mtl);
        if (awdMtl == NULL) {
            int i=0;
            char * matName_ptr = W2A(mtl->GetName());
            awdMtl = new AWDMaterial(matName_ptr, strlen(matName_ptr));
            free(matName_ptr);
            bool hasDifftex=false;
            bool hasAmbTex=false;
            bool hasSpecTex=false;
            bool hasNormalTex=false;
            double specularLevel=1.0;
            double glossLevel=50;
            double ambientLevel=1.0;
            double alpha=1.0;
            AWDBitmapTexture *awdNormalTex=NULL;
            AWDBitmapTexture *awdDiffTex=NULL;
            AWDBitmapTexture *awdAmbTex=NULL;
            AWDBitmapTexture *awdSpecTex=NULL;
            StdMat2 * stdMat2=(StdMat2 *)mtl;
            if (stdMat2!=NULL){
                if (stdMat2->IsFaceted()){
                    awdMtl->set_is_faceted(true);
                }
                Shader * thisShader=stdMat2->GetShader();
                if (thisShader!=NULL){
                    MSTR className;
                    thisShader->GetClassName(className);
                    char * thisShader_ptr=W2A(className);
                    specularLevel=double(double(thisShader->GetSpecularLevel(0)));
                    glossLevel=thisShader->GetGlossiness(0)*1000;
                    if (ATTREQ(thisShader_ptr,"Blinn")){//ambientLevel=1.0;
                        // standart material blinn
                    }
                    else if (ATTREQ(thisShader_ptr,"Anisotropic")){
                        // Anisotropic spec shading
                        awdMtl->add_method(new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC));
                    }
                    else if (ATTREQ(thisShader_ptr,"Metal")){
                        // Metal material
                    }
                    else if (ATTREQ(thisShader_ptr,"Multi-Layer")){
                        // Multi-Layer material
                    }
                    else if (ATTREQ(thisShader_ptr,"Oren-Nayar-Blinn")){
                        // Oren-Nayar-Blinn material
                    }
                    else if (ATTREQ(thisShader_ptr,"Phong")){
                        awdMtl->add_method(new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_PHONG));
                        // Phong material
                    }
                    else if (ATTREQ(thisShader_ptr,"Strauss")){
                        // Strauss material
                    }
                    else if (ATTREQ(thisShader_ptr,"Translucent Shader")){
                        // Strauss material
                    }
                    free(thisShader_ptr);
                }
                //int thisShading=stdMat2->GetShading();
            }
            bool exportTexMaterials=true;
            int saveType=opts->TextureMode();
            if ((saveType!=0)||(exportTexMaterials)){
                for (i=0; i<mtl->NumSubTexmaps(); i++) {
                    Texmap *tex = mtl->GetSubTexmap(i);
                    // If there is a texture, AND that texture is a plain bitmap
                    if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                        char * slotName_ptr=W2A(mtl->GetSubTexmapSlotName(i));
                        if (ATTREQ(slotName_ptr,"Diffuse Color")){
                            awdDiffTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE, MATDIFFUSE);
                            hasDifftex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Ambient Color")){
                            awdAmbTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE, MATAMBIENT);
                            hasAmbTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Specular Color")){
                            awdSpecTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE, MATSPECULAR);
                            hasSpecTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Specular Level")){
                            awdSpecTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE, MATSPECULAR);
                            hasSpecTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Glossiness")){
                            awdSpecTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE, MATSPECULAR);
                            hasSpecTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Bump")){
                            awdNormalTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE, MATNORMAL);
                            hasNormalTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        free(slotName_ptr);
                    }

                    if (tex != NULL && tex->ClassID() == Class_ID(0x243e22c6, 0x63f6a014)) { // GNORMAL_CLASS_ID not supüported in 2011
                        char * slotName_ptr=W2A(mtl->GetSubTexmapSlotName(i));
                        if (ATTREQ(slotName_ptr,"Bump")){
                            // we consider all maps found in the bumb_node as normal-maps, without checking the mode of the node.
                            IParamBlock2 * pb = tex->GetParamBlockByID(0); //gnormal_params = 0  not supüported in 2011
                            int p=0;
                            for (p=0; p<pb->NumParams(); p++) {
                                ParamID pid = pb->IndextoID(p);
                                ParamDef def = pb->GetParamDef(pid);
                                if (pb->GetParameterType(pid)==TYPE_TEXMAP){
                                    char * defName_ptr=W2A(def.int_name);
                                    if (ATTREQ(defName_ptr,"normal_map")){
                                        if (!hasNormalTex){
                                            Texmap * newNormalTexMap=(Texmap *)pb->GetTexmap(pid);
                                            if (newNormalTexMap != NULL && newNormalTexMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)){
                                                awdNormalTex = ExportBitmapTexture((BitmapTex *)newNormalTexMap, awdMtl, UNDEFINEDTEXTYPE, MATNORMAL);
                                                hasNormalTex=true;
                                            }
                                        }
                                        RETURN_VALUE_IF_ERROR(NULL);
                                    }
                                    free(defName_ptr);
                                }
                            }
                        }
                        free(slotName_ptr);
                    }
                }
                if (hasDifftex || hasAmbTex){
                    awdMtl->set_type(AWD_MATTYPE_TEXTURE);
                    if ((hasDifftex)&&(awdDiffTex!=NULL)&&(awdDiffTex->get_isValid())){
                        awdMtl->set_texture(awdDiffTex);}
                    if ((hasAmbTex)&&(awdAmbTex!=NULL)&&(awdAmbTex->get_isValid())){
                        awdMtl->set_ambientTexture(awdAmbTex);}
                }
                else{
                    awdMtl->set_type(AWD_MATTYPE_COLOR);
                }
            }
            else{
                awdMtl->set_type(AWD_MATTYPE_COLOR);
            }

            awdMtl->set_color(convertColor(mtl->GetDiffuse(0).toRGB()));
            awdMtl->set_ambientColor(convertColor(mtl->GetAmbient(0).toRGB()));
            awdMtl->set_specularColor(convertColor(mtl->GetSpecular(0).toRGB()));
            awdMtl->set_both_sides(mtl->GetTwoSided());
            awdMtl->set_alpha(mtl->GetOpacity(0));

            awdMtl->set_ambientStrength(ambientLevel);
            awdMtl->set_specularStrength(specularLevel);
            awdMtl->set_glossStrength(glossLevel);
            // this can optionally overwrite previous defined material-settings
            GetCustomAWDMaterialSettings(mtl, awdMtl);
            if ((hasSpecTex)&&(awdSpecTex!=NULL)&&(awdSpecTex->get_isValid())){
                awdMtl->set_specTexture(awdSpecTex);}
            if ((hasNormalTex)&&(awdNormalTex!=NULL)&&(awdNormalTex->get_isValid())){
                awdMtl->set_normalTexture(awdNormalTex);}
            awd->add_material(awdMtl);
            materialCache->Set(mtl, awdMtl);
        }
    }
    //output_debug_string("         -->finished ExportOneMaterial");
    return awdMtl;
}