#include "maxawdexporter.h"

void MaxAWDExporter::ReadAWDShadowsMethods(Modifier *node_mod, AWDBlock * light){
    int num_params = node_mod->NumParamBlocks();
    int cntBlocks=0;
    IParamBlock2* pb ;
    char * settingsNodeID_ptr = NULL;
    bool simpleMode=true;
    int animType=0;
    int p=0;
    bool export_shadows=false;
    bool use_radius=false;
    bool use_falloff=false;
    bool use_specular=false;
    bool use_ambient=false;
    bool use_diffuse=false;
    double this_radius=false;
    double this_falloff=false;
    double this_specular=false;
    double this_ambient=false;
    double this_diffuse=false;
    awd_color ambientColor=0xffffff;
    bool setAmbientColor=false;
    AWDLight * awdLight=(AWDLight*)light;
    pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        int numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);
            ParamType2 paramtype = pb->GetParameterType(pid);
            char * paramName=W2A(def.int_name);
            if (paramtype==TYPE_BOOL){
                if (ATTREQ(paramName, "use_radius"))
                    use_radius = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "use_falloff"))
                    use_falloff = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "use_specular"))
                    use_specular = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "use_ambient"))
                    use_ambient = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "use_diffuse"))
                    use_diffuse = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "export_shadows"))
                    export_shadows = (0 != pb->GetInt(pid));
            }
            if (paramtype==TYPE_FLOAT){
                if (ATTREQ(paramName, "this_radius"))
                    this_radius = pb->GetFloat(pid);
                if (ATTREQ(paramName, "this_falloff"))
                    this_falloff = pb->GetFloat(pid);
                if (ATTREQ(paramName, "this_specular"))
                    this_specular = pb->GetFloat(pid);
                if (ATTREQ(paramName, "this_ambient"))
                    this_ambient = pb->GetFloat(pid);
                if (ATTREQ(paramName, "this_ambient"))
                    this_ambient = pb->GetFloat(pid);
                if (ATTREQ(paramName, "this_diffuse"))
                    this_diffuse = pb->GetFloat(pid);
            }
            if (paramtype==TYPE_STRING){
                if (ATTREQ(paramName, "thisAWDID"))
                    settingsNodeID_ptr = W2A(pb->GetStr(pid));
            }
            if (paramtype==TYPE_RGBA){
                if (ATTREQ(paramName,"this_ambientColor")){
                    Point3 ambientColormax = pb->GetColor(pid);
                    setAmbientColor=true;
                    awd_color ambientColor = createARGB(255, ambientColormax.x*255, ambientColormax.y*255, ambientColormax.z*255);
                }
            }
            free(paramName);
        }
        if(use_radius)
            awdLight->set_radius(this_radius);
        if(use_falloff)
            awdLight->set_falloff(this_falloff);
        if(use_specular)
            awdLight->set_specular(this_specular);
        if(use_ambient){
            awdLight->set_ambient(this_ambient);
            if (setAmbientColor)
                awdLight->set_ambientColor(ambientColor);
        }
        if(use_diffuse)
            awdLight->set_diffuse(this_diffuse);
    }
    else{
        return;
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    if((export_shadows)&&(opts->IncludeShadows())){
        int shadowMapper=0;
        int shadowMethod=0;
        int baseShadowMethod=0;
        int mapper_depthMapSize=0;
        int mapper_cascades=0;
        bool autoApply=false;
        double mapper_coverage=0;
        double shadow_alpha=0;
        double shadow_epsilon=0;
        int shadow_samples=0;
        double shadow_range=0;
        pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 1);
        if(pb!=NULL){
            int numBlockparams=pb->NumParams();
            for (p=0; p<numBlockparams; p++) {
                ParamID pid = pb->IndextoID(p);
                ParamDef def = pb->GetParamDef(pid);
                ParamType2 paramtype = pb->GetParameterType(pid);
                char * paramName=W2A(def.int_name);
                if (paramtype==TYPE_BOOL){
                    if (ATTREQ(paramName, "auto_apply"))
                        autoApply = (0 != pb->GetInt(pid));
                }
                if (paramtype==TYPE_FLOAT){
                    if (ATTREQ(paramName, "mapper_coverage"))
                        mapper_coverage = pb->GetFloat(pid);
                    if (ATTREQ(paramName, "shadow_alpha"))
                        shadow_alpha = pb->GetFloat(pid);
                    if (ATTREQ(paramName, "shadow_epsilon"))
                        shadow_epsilon = pb->GetFloat(pid);
                    if (ATTREQ(paramName, "shadow_range"))
                        shadow_range = pb->GetFloat(pid);
                }
                if (paramtype==TYPE_INT){
                    if (ATTREQ(paramName, "shadow_samples"))
                        shadow_samples = pb->GetInt(pid);
                    if (ATTREQ(paramName, "shadowMapper"))
                        shadowMapper = pb->GetInt(pid);
                    if (ATTREQ(paramName, "ShadowMethod"))
                        shadowMethod = pb->GetInt(pid);
                    if (ATTREQ(paramName, "baseShadowMethod"))
                        baseShadowMethod = pb->GetInt(pid);
                    if (ATTREQ(paramName, "mapper_depthMapSize"))
                        mapper_depthMapSize = pb->GetInt(pid);
                    if (ATTREQ(paramName, "mapper_cascades"))
                        mapper_cascades = pb->GetInt(pid);
                }
                free(paramName);
            }
        }
        if(shadowMapper!=AWD_SHADOW_MAPPER_UNDEFINED){
            char *shadowName_ptr = (char*)malloc(strlen(awdLight->get_name())+8);
            strcpy(shadowName_ptr, awdLight->get_name());
            strcat(shadowName_ptr, "_shadow");
            AWDShadowMethod * newShadowMethod = new AWDShadowMethod(shadowName_ptr, strlen(shadowName_ptr), AWD_SHADOW_UNDEFINED);
            newShadowMethod->set_autoApply(autoApply);
            newShadowMethod->set_awdLight(awdLight);
            char *shadowName_base_ptr = (char*)malloc(strlen(shadowName_ptr)+8);
            strcpy(shadowName_base_ptr, shadowName_ptr);
            strcat(shadowName_base_ptr, "_base");
            AWDShadowMethod * newShadowBaseMethod = new AWDShadowMethod(shadowName_base_ptr, strlen(shadowName_base_ptr), AWD_SHADOW_UNDEFINED);
            free(shadowName_ptr);
            free(shadowName_base_ptr);
            newShadowBaseMethod->set_awdLight(awdLight);
            mapper_depthMapSize-=1;
            if(awdLight->get_light_type()==AWD_LIGHT_POINT){
                shadowMapper=AWD_SHADOW_MAPPER_CUBE;
                if(mapper_depthMapSize==2)
                    mapper_depthMapSize=3;
                awdLight->set_shadowmapper_depth_size(mapper_depthMapSize);
                newShadowMethod->set_shadow_type(AWD_HARD_SHADOW);
                newShadowMethod->add_number_shadow_prop(PROPS_NUMBER1, shadow_alpha, 1.0);
                newShadowMethod->add_number_shadow_prop(PROPS_NUMBER2, shadow_epsilon, 0.002);
            }
            else if(awdLight->get_light_type()==AWD_LIGHT_DIR){
                awdLight->set_shadowmapper_depth_size(mapper_depthMapSize);
                if(shadowMapper==AWD_SHADOW_MAPPER_DIRECTIONAL){
                    if(shadowMethod==1){
                        newShadowMethod->set_shadow_type(AWD_FILTERED_SHADOW);
                    }
                    if(shadowMethod==2){
                        newShadowMethod->set_shadow_type(AWD_DITHERED_SHADOW);
                    }
                    if(shadowMethod==3){
                        newShadowMethod->set_shadow_type(AWD_SOFT_SHADOW);
                    }
                    if(shadowMethod==4){
                        newShadowMethod->set_shadow_type(AWD_HARD_SHADOW);
                    }
                    newShadowMethod->add_number_shadow_prop(PROPS_NUMBER1, shadow_alpha, 1.0);
                    newShadowMethod->add_number_shadow_prop(PROPS_NUMBER2, shadow_epsilon, 0.002);
                    if((shadowMethod==2)||(shadowMethod==3)){
                        newShadowMethod->add_number_shadow_prop(PROPS_NUMBER3, shadow_range, 1.0);
                        newShadowMethod->add_int_shadow_prop(201, shadow_samples, 5);
                    }
                }
                else if(shadowMapper==2){//AWD_SHADOW_MAPPER_CASCADE){
                    shadowMapper=AWD_SHADOW_MAPPER_CASCADE;
                    newShadowMethod->set_shadow_type(AWD_CASCADE_SHADOW);
                    awdLight->set_shadowmapper_cascades(mapper_cascades);
                }
                else if(shadowMapper==3){
                    shadowMapper=AWD_SHADOW_MAPPER_NEAR;
                    newShadowMethod->set_shadow_type(AWD_NEAR_SHADOW);
                    awdLight->set_shadowmapper_coverage(mapper_coverage);
                }
                if((shadowMapper==AWD_SHADOW_MAPPER_NEAR)||(shadowMapper==AWD_SHADOW_MAPPER_CASCADE)){
                    if(baseShadowMethod==1){
                        newShadowBaseMethod->set_shadow_type(AWD_FILTERED_SHADOW);
                    }
                    if(baseShadowMethod==2){
                        newShadowBaseMethod->set_shadow_type(AWD_DITHERED_SHADOW);
                    }
                    if(baseShadowMethod==3){
                        newShadowBaseMethod->set_shadow_type(AWD_SOFT_SHADOW);
                    }
                    if(baseShadowMethod==4){
                        newShadowBaseMethod->set_shadow_type(AWD_HARD_SHADOW);
                    }
                    newShadowBaseMethod->add_number_shadow_prop(PROPS_NUMBER1, shadow_alpha, 1.0);
                    newShadowBaseMethod->add_number_shadow_prop(PROPS_NUMBER2, shadow_epsilon, 0.002);
                    if((baseShadowMethod==2)||(baseShadowMethod==3)){
                        newShadowBaseMethod->add_number_shadow_prop(PROPS_NUMBER3, shadow_range, 1.0);
                        newShadowBaseMethod->add_int_shadow_prop(201, shadow_samples, 5);
                    }
                }
            }

            awdLight->set_shadowmapper_type((AWD_shadow_mapper_type)shadowMapper);
            if(newShadowMethod->get_shadow_type()!=AWD_SHADOW_UNDEFINED){
                awdLight->set_shadowMethod(newShadowMethod);
                if(newShadowBaseMethod->get_shadow_type()!=AWD_SHADOW_UNDEFINED){
                    if((newShadowMethod->get_shadow_type()==AWD_NEAR_SHADOW)||(newShadowMethod->get_shadow_type()==AWD_CASCADE_SHADOW)){
                        newShadowMethod->set_base_method(newShadowBaseMethod);
                    }
                    awd->add_shadow(newShadowBaseMethod);
                }
                shadowMethodsCache->Set(settingsNodeID_ptr, newShadowMethod);
                awd->add_shadow(newShadowMethod);
            }
            if(newShadowMethod->get_shadow_type()==AWD_SHADOW_UNDEFINED)
                delete newShadowMethod;
            if(newShadowBaseMethod->get_shadow_type()==AWD_SHADOW_UNDEFINED)
                delete newShadowBaseMethod;
        }
    }
    if (settingsNodeID_ptr!=NULL)
        free(settingsNodeID_ptr);
}