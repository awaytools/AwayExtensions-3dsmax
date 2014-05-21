#include "maxawdexporter.h"

AWDBlock * MaxAWDExporter::ExportCameraAndTextureExporter(INode * node, double * mtxData, AWDSceneBlock * parent, BlockSettings * blockSettings)
{
    awd_float64 * transform_mtx_camera = (double *)malloc(12*sizeof(awd_float64));
    awd_float64 store1 = mtxData[3];
    awd_float64 store2 = mtxData[4];
    awd_float64 store3 = mtxData[5];
    transform_mtx_camera[0] = mtxData[0];
    transform_mtx_camera[1] = mtxData[1];
    transform_mtx_camera[2] = mtxData[2];
    transform_mtx_camera[3] = mtxData[6];
    transform_mtx_camera[4] = mtxData[7];
    transform_mtx_camera[5] = mtxData[8];
    transform_mtx_camera[6] = store1*-1;
    transform_mtx_camera[7] = store2*-1;
    transform_mtx_camera[8] = store3*-1;
    transform_mtx_camera[9] = mtxData[9];
    transform_mtx_camera[10] = mtxData[10];
    transform_mtx_camera[11] = mtxData[11];
    Object *obj;
    obj = node->GetObjectRef();
    SClass_ID sid=obj->SuperClassID();
    getBaseObjectAndID( obj, sid );
    CameraObject *camObject= (CameraObject *) obj;
    double fov=camObject->GetFOV(0);
    bool isOrtho=camObject->IsOrtho();
    double clipNear=camObject->GetClipDist(0,CAM_HITHER_CLIP);
    double clipFar=camObject->GetClipDist(0,CAM_YON_CLIP);
    char * camName_ptr=W2A(node->GetName());
    AWD_lens_type camType=AWD_LENS_PERSPECTIVE;
    if (isOrtho)
        camType=AWD_LENS_ORTHO;
    AWDCamera * awdCamera = new AWDCamera(camName_ptr, strlen(camName_ptr), camType, transform_mtx_camera);
    AWDTextureProjector * textureProjector= new AWDTextureProjector(camName_ptr, strlen(camName_ptr), mtxData);
    AWDBitmapTexture * projectionTexture = NULL;
    free(camName_ptr);
    if(!isOrtho){
        //double aspectRatio=maxInterface->GetRendApect();
        double aspectRatio=1/double(maxInterface->GetRendImageAspect());
        double horizontalFOV=double(fov* (double(double(180)/(double(3.14159265358979323846)))));
        double verticalFOV=horizontalFOV * double(aspectRatio);
        awdCamera->set_lens_fov(verticalFOV);
    }

    awdCamera->set_lens_near(clipNear * blockSettings->get_scale());
    awdCamera->set_lens_far(clipFar * blockSettings->get_scale());
    bool exportCamera=true;
    bool exportTextureProjector=false;

    BaseObject* node_bo = (BaseObject*)node->GetObjectRef();
    IDerivedObject* node_der = NULL;
    char * settingsNodeID_ptr=NULL;

    if((node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == DERIVOB_CLASS_ID ))
    {
        node_der = ( IDerivedObject* ) node->GetObjectRef();
        if (node_der!=NULL){
            int nMods = node_der->NumModifiers();
            for (int m = 0; m<nMods; m++){
                Modifier* node_mod = node_der->GetModifier(m);
                if (node_mod->IsEnabled()){
                    MSTR className;
                    node_mod->GetClassName(className);
                    char * className_ptr=W2A(className);
                    if (ATTREQ(className_ptr,"AWDCamera")){
                        IParamBlock2* pb = GetParamBlock2ByName((ReferenceMaker*)node_mod, "main");
                        if(pb!=NULL){
                            int numBlockparams=pb->NumParams();
                            int p=0;
                            for (p=0; p<numBlockparams; p++) {
                                ParamID pid = pb->IndextoID(p);
                                ParamDef def = pb->GetParamDef(pid);
                                ParamType2 paramtype = pb->GetParameterType(pid);
                                char * paramName=W2A(def.int_name);
                                if (paramtype==TYPE_STRING) {
                                    if (ATTREQ(paramName, "thisAWDID"))
                                        settingsNodeID_ptr = W2A(pb->GetStr(pid));
                                }
                                if (paramtype==TYPE_BOOL){
                                    if (ATTREQ(paramName, "exportCamera"))
                                        exportCamera = (0 != pb->GetInt(pid));
                                    if (ATTREQ(paramName, "exportTextureProjector"))
                                        exportTextureProjector = (0 != pb->GetInt(pid));
                                }
                                free(paramName);
                            }
                        }
                        if(exportCamera){
                            AWD_lens_type lens_type = AWD_LENS_PERSPECTIVE;
                            int lensType=1;
                            int projectionHeight=1;
                            int offcenterX_pos=1;
                            int offcenterX_neg=1;
                            int offcenterY_pos=1;
                            int offcenterY_neg=1;
                            IParamBlock2*  pb = GetParamBlock2ByName((ReferenceMaker*)node_mod, "camera_params");
                            if(pb!=NULL){
                                int numBlockparams=pb->NumParams();
                                int p=0;
                                for (p=0; p<numBlockparams; p++) {
                                    ParamID pid = pb->IndextoID(p);
                                    ParamDef def = pb->GetParamDef(pid);
                                    ParamType2 paramtype = pb->GetParameterType(pid);
                                    char * paramName=W2A(def.int_name);
                                    if (paramtype==TYPE_INT){
                                        if (ATTREQ(paramName, "lensType"))
                                            lensType = pb->GetInt(pid);
                                        if (ATTREQ(paramName, "projectionHeight"))
                                            projectionHeight = pb->GetInt(pid);
                                        if (ATTREQ(paramName, "offcenterX_pos"))
                                            offcenterX_pos = pb->GetInt(pid);
                                        if (ATTREQ(paramName, "offcenterX_neg"))
                                            offcenterX_neg = pb->GetInt(pid);
                                        if (ATTREQ(paramName, "offcenterY_pos"))
                                            offcenterY_pos = pb->GetInt(pid);
                                        if (ATTREQ(paramName, "offcenterY_neg"))
                                            offcenterY_neg = pb->GetInt(pid);
                                    }
                                    free(paramName);
                                }
                                if(lensType==2){
                                    lens_type=AWD_LENS_ORTHO;
                                    awdCamera->set_lens_proj_height(projectionHeight);
                                }
                                else if (lensType==3){
                                    lens_type=AWD_LENS_ORTHOOFFCENTER;
                                    awdCamera->set_lens_offset(offcenterX_pos, offcenterX_neg, offcenterY_neg, offcenterY_pos );
                                }
                                awdCamera->set_lens_type(lens_type);
                            }
                        }
                        if(exportTextureProjector){
                            IParamBlock2*  pb = GetParamBlock2ByName((ReferenceMaker*)node_mod, "texture_projector_params");
                            if(pb!=NULL){
                                int numBlockparams=pb->NumParams();
                                int p=0;
                                for (p=0; p<numBlockparams; p++) {
                                    ParamID pid = pb->IndextoID(p);
                                    ParamDef def = pb->GetParamDef(pid);
                                    ParamType2 paramtype = pb->GetParameterType(pid);
                                    char * paramName=W2A(def.int_name);
                                    if (paramtype==TYPE_TEXMAP){
                                        if (ATTREQ(paramName,"projectionTexture") ){
                                            Texmap *projectionTexmap = pb->GetTexmap(pid);
                                            if (projectionTexmap != NULL && projectionTexmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                                projectionTexture=ExportBitmapTexture((BitmapTex *)projectionTexmap, NULL, UNDEFINEDTEXTYPE, FORTEXTUREPROJECTOR);
                                                if (projectionTexture!=NULL)
                                                    textureProjector->set_texture(projectionTexture);
                                                else{
                                                    textureProjector->make_invalide();
                                                    exportTextureProjector=false;
                                                }
                                            }
                                        }
                                    }
                                    if (paramtype==TYPE_FLOAT){
                                        if (ATTREQ(paramName, "aspect_ratio"))
                                            textureProjector->set_aspect_ratio(pb->GetFloat(pid));
                                    }
                                    free(paramName);
                                }
                            }
                            else{
                                textureProjector->make_invalide();
                                exportTextureProjector=false;
                            }
                        }
                    }
                    free(className_ptr);
                }
            }
        }
    }
    else{
    }
    if(exportCamera){
        if (parent) {
            parent->add_child(awdCamera);
        }
        else {
            awd->add_scene_block(awdCamera);
        }
    }
    else{
        delete awdCamera;
    }

    if(exportTextureProjector){
        textureProjectorCache->Set(settingsNodeID_ptr, textureProjector);
        if (parent) {
            parent->add_child(textureProjector);
        }
        else {
            awd->add_scene_block(textureProjector);
        }
        if(projectionTexture!=NULL)
            awd->add_texture(projectionTexture);
    }
    else{
        delete textureProjector;
        if(projectionTexture!=NULL)
            delete projectionTexture;
    }
    if(settingsNodeID_ptr!=NULL)
        free(settingsNodeID_ptr);
    if((exportCamera)&&(!exportTextureProjector))
        return awdCamera;
    else if((!exportCamera)&&(exportTextureProjector)&&(textureProjector!=NULL))
        return textureProjector;
    else if((exportCamera)&&(exportTextureProjector))
        return awdCamera;
    return NULL;
}