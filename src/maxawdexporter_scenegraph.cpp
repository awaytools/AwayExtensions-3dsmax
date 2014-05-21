#include "maxawdexporter.h"
#include <iInstanceMgr.h>
void MaxAWDExporter::PreProcessSceneGraph(INode *node, bool parentExcluded, BlockSettings* blockSettings)
{
    Object *obj;
    obj = node->GetObjectRef();
    bool excludeThis=parentExcluded;
    bool excludeChild=parentExcluded;
    bool isReference=false;
    if (obj){
        SClass_ID sid=obj->SuperClassID();
        // no matter if set to export or  "selectedObject"-export,
        // we need to collect the AWDAnimationNodes,
        // if we want Animation to be savly exported for a selected object.
        BaseObject* node_bo = (BaseObject*)node->GetObjectRef();
        IDerivedObject* node_der = NULL;

        if((node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == DERIVOB_CLASS_ID ))
        {
            char * name=W2A(node->GetName());
            free(name);
            bool isReference=true;
            SClass_ID sid=obj->SuperClassID();
            ULONG handle = 0;
            obj->NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
            INode *originalNode = GetCOREInterface()->GetINodeByHandle(handle);
            Object * thisBaseObj = originalNode->GetObjectRef();

            node_der = ( IDerivedObject* ) node->GetObjectRef();

            // prevent reading AWD-modifier from the same instance
            CustomAttributes_struct custAWDSettings;
            custAWDSettings = GetCustomAWDObjectSettings(node_der, obj);
            if (!custAWDSettings.export_this){
                this->hasExcludedObjects=true;
                allExcludedCache->Set(obj, true);
            }
            bool isAllreadyCached=allExcludedCache->Get(node_der);
            if (!isAllreadyCached){
                if (node_der!=NULL){
                    int nMods = node_der->NumModifiers();
                    for (int m = 0; m<nMods; m++){
                        Modifier* node_mod = node_der->GetModifier(m);
                        if (node_mod->IsEnabled()){
                            MSTR className;
                            node_mod->GetClassName(className);
                            char * className_ptr=W2A(className);
                            if (ATTREQ(className_ptr,"AWDEffectMethod")){
                                if (opts->IncludeMethods())
                                    ReadAWDEffectMethods(node_mod, node);
                            }
                            else if (ATTREQ(className_ptr,"AWDAnimationSet")){
                                if ((opts->ExportSkelAnim())||(opts->ExportVertexAnim())){
                                    ReadAWDAnimSet(node_mod);
                                }
                            }
                            else if (ATTREQ(className_ptr,"AWDSkeleton")){
                                allExcludedCache->Set(obj, true);
                                this->hasExcludedObjects=true;
                                ReadAWDSkeletonMod(node_mod, node);
                            }
                            else if (ATTREQ(className_ptr,"AWDVertexAnimSource")){
                                if (opts->ExportVertexAnim())
                                    ReadAWDVertexMod(node_mod, node);
                            }
                            else if (ATTREQ(className_ptr,"AWDSkeletonClone")){
                                allExcludedCache->Set(obj, true);
                                this->hasExcludedObjects=true;
                                if (opts->ExportSkelAnim())
                                    ReadAWDAnimSourceCloneMod(node_mod, node, ANIMTYPESKELETON);
                            }
                            else if (ATTREQ(className_ptr,"AWDVertexAnimSourceClone")){
                                allExcludedCache->Set(obj, true);
                                this->hasExcludedObjects=true;
                                if (opts->ExportVertexAnim())
                                    ReadAWDAnimSourceCloneMod(node_mod, node, ANIMTYPEVERTEX);
                            }
                            free(className_ptr);
                        }
                    }
                }
            }
            // no skin = no skeleton animation
            IDerivedObject* thisBaseObj_der = (IDerivedObject*)(node->GetObjectRef());
            Object * thisOBJ=(Object *)thisBaseObj_der->GetObjRef();
            if((thisOBJ->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == DERIVOB_CLASS_ID )){
                IDerivedObject* thisDerObj=( IDerivedObject* ) thisOBJ;
                if (thisDerObj!=NULL){
                    int nMods = thisDerObj->NumModifiers();
                    for (int m = 0; m<nMods; m++){
                        Modifier* node_mod = thisDerObj->GetModifier(m);
                        if (node_mod->IsEnabled()){
                            MSTR className;
                            node_mod->GetClassName(className);
                            char * className_ptr=W2A(className);
                            if (ATTREQ(className_ptr,"AWDEffectMethod")){
                                if (opts->IncludeMethods())
                                    ReadAWDEffectMethods(node_mod, node);
                            }
                            else if (ATTREQ(className_ptr,"AWDAnimationSet")){
                                if ((opts->ExportSkelAnim())||(opts->ExportVertexAnim())){
                                    ReadAWDAnimSet(node_mod);
                                }
                            }
                            else if (ATTREQ(className_ptr,"AWDSkeleton")){
                                allExcludedCache->Set(obj, true);
                                this->hasExcludedObjects=true;
                                ReadAWDSkeletonMod(node_mod, node);
                            }
                            else if (ATTREQ(className_ptr,"AWDVertexAnimSource")){
                                if (opts->ExportVertexAnim())
                                    ReadAWDVertexMod(node_mod, node);
                            }
                            else if (ATTREQ(className_ptr,"AWDSkeletonClone")){
                                allExcludedCache->Set(obj, true);
                                this->hasExcludedObjects=true;
                                if (opts->ExportSkelAnim())
                                    ReadAWDAnimSourceCloneMod(node_mod, node, ANIMTYPESKELETON);
                            }
                            else if (ATTREQ(className_ptr,"AWDVertexAnimSourceClone")){
                                allExcludedCache->Set(obj, true);
                                this->hasExcludedObjects=true;
                                if (opts->ExportVertexAnim())
                                    ReadAWDAnimSourceCloneMod(node_mod, node, ANIMTYPEVERTEX);
                            }
                            free(className_ptr);
                        }
                    }
                }
            }
        }
        ILayer* thisLayer=(ILayer*)node->GetReference(NODE_LAYER_REF);
        ILayerProperties* thisLayerprops=(ILayerProperties*)thisLayer->GetInterface(LAYERPROPERTIES_INTERFACE);
        bool isSelectedLayer=thisLayerprops->getCurrent();
        bool isEnabledLayer=thisLayerprops->getOn();
        if((!isSelectedLayer)&&(opts->ExcludeUnselectedLayers())){
            this->hasExcludedLayers=true;
            allExcludedCache->Set(obj, true);
        }
        if((!isEnabledLayer)&&(opts->ExcludeInvisibleLayers())){
            this->hasExcludedLayers=true;
            allExcludedCache->Set(obj, true);
        }
        // if the node is allready excluded from export by its parent, we dont need to read its custom AWDObjectSettings
        // and we dont need to check if its a light either...
        if (excludeThis){
            allExcludedCache->Set(obj, true);
            this->hasExcludedObjects=true;
        }
        else{
            BaseObject * node_bo = node->GetObjectRef();
            CustomAttributes_struct custAWDSettings;
            if(node_der!=NULL)
            {
                Object * newObj = node_der->GetObjRef();
                custAWDSettings = GetCustomAWDObjectSettings(node_der, newObj);
            }
            else{
                custAWDSettings = GetCustomAWDObjectSettings(NULL, obj);
            }
            if (!custAWDSettings.export_this){
                allExcludedCache->Set(obj, true);
                this->hasExcludedObjects=true;
            }
            excludeChild=false;
            if (!custAWDSettings.export_this_children){
                excludeChild=true;
            }
            obj = node->GetObjectRef();
            sid=obj->SuperClassID();
            getBaseObjectAndID(obj, sid);
            // check if the node is a light, and if so, create a entry in the lightCache.
            if (sid==LIGHT_CLASS_ID){
                if (opts->IncludeLights()){
                    Interval valid;
                    AWD_light_type light_type= AWD_LIGHT_UNDEFINED;
                    if (obj->ClassID()==Class_ID(OMNI_LIGHT_CLASS_ID,0)){
                        light_type=AWD_LIGHT_POINT;
                    }
                    else if (obj->ClassID()==Class_ID(DIR_LIGHT_CLASS_ID,0)) {
                        light_type=AWD_LIGHT_DIR;
                    }
                    else if (obj->ClassID()==Class_ID(SPOT_LIGHT_CLASS_ID,0)) {
                        //ERROR - light will be converted to omni-light(pointlight)
                        light_type=AWD_LIGHT_POINT;
                    }
                    else if (obj->ClassID()==Class_ID(FSPOT_LIGHT_CLASS_ID,0)) {
                        //ERROR - light will be converted to omni-light(pointlight)
                        light_type=AWD_LIGHT_POINT;
                    }
                    if (light_type==AWD_LIGHT_UNDEFINED){
                        char * lightName_ptr=W2A(node->GetName());
                        AWDMessageBlock * newWarning = new AWDMessageBlock(lightName_ptr, "Unsupported type of light - not exported");
                        awd->get_message_blocks()->append(newWarning);
                        free(lightName_ptr);
                    }
                    else if (light_type!=AWD_LIGHT_UNDEFINED){
                        char * lightName_ptr=W2A(node->GetName());
                        AWDLight * awdLight=new AWDLight(lightName_ptr, strlen(lightName_ptr));
                        free(lightName_ptr);
                        awdLight->set_light_type(light_type);
                        LightObject * lt= (LightObject *)obj;
                        GenLight *gl = (GenLight *)obj;
                        LightState ls;
                        lt->EvalLightState(0, valid, &ls);
                        ExclList * excludeList=lt->GetExclList();
                        bool includeObjs=lt->Include();
                        double diffuse=lt->GetIntensity(0);
                        Point3 color = lt->GetRGBColor(0);
                        awd_color awdColor = createARGB(255, color.x*255, color.y*255, color.z*255);

                        double diffuseStrength=diffuse;
                        bool affectDiffuse=gl->GetAffectDiffuse();
                        if(!affectDiffuse)
                            diffuseStrength=0.0;
                        double specStrength=diffuse;
                        bool affectSpec=gl->GetAffectSpecular();
                        if(!affectSpec)
                            specStrength=0.0;
                        double ambientStrength=0.0;
                        bool ambientOnly=gl->GetAmbientOnly();
                        if(ambientOnly){
                            specStrength=0.0;
                            diffuseStrength=0.0;
                            ambientStrength=diffuse;
                        }
                        if (light_type==AWD_LIGHT_POINT){
                            bool hasAtten=gl->GetUseAtten();
                            double radius= 90000.0;
                            double falloff=100000.0;
                            if(hasAtten){
                                radius=gl->GetAtten(0, ATTEN_START)*blockSettings->get_scale();
                                falloff=gl->GetAtten(0, ATTEN_END)*blockSettings->get_scale();
                            }
                            awdLight->set_radius(radius);
                            awdLight->set_falloff(falloff);
                        }
                        awdLight->set_color(awdColor);
                        awdLight->set_ambientColor(awdColor);
                        awdLight->set_diffuse(diffuseStrength);
                        awdLight->set_specular(specStrength);
                        awdLight->set_ambient(ambientStrength);
                        if (node_der!=NULL){
                            int nMods = node_der->NumModifiers();
                            for (int m = 0; m<nMods; m++){
                                Modifier* node_mod = node_der->GetModifier(m);
                                if (node_mod->IsEnabled()){
                                    MSTR className;
                                    node_mod->GetClassName(className);
                                    char * className_ptr=W2A(className);
                                    if (ATTREQ(className_ptr,"AWDShadowMethod")){
                                        if (opts->IncludeMethods())
                                            ReadAWDShadowsMethods(node_mod, awdLight);
                                    }
                                    free(className_ptr);
                                }
                            }
                        }
                        lightCache->Set(node,awdLight,excludeList, includeObjs);
                    }
                }
            }
        }
        // We want to exclude all objects that are used as bones.
        // So we check for skin-modifier, no matter if the skinned object will be exported or not.
        // Get the skin - find all bones for the skin - add all the bones to the excluded cache:
        IDerivedObject *derivedObject = NULL;
        int skinIdx;
        ObjectState os;
        skinIdx = IndexOfSkinMod(node->GetObjectRef(), &derivedObject);
        ISkin *skin = NULL;
        if (derivedObject != NULL && skinIdx >= 0) {
            Modifier *mod = derivedObject->GetModifier(skinIdx);
            skin = (ISkin *)mod->GetInterface(I_SKIN);
        }
        if (skin && skin->GetNumBones()){
            int numBones=skin->GetNumBones();
            for (int i=0;i<numBones;i++){
                INode* curBone=skin->GetBone(i);
                Object* thisobj=curBone->GetObjectRef();
                allBonesCache->Set(thisobj, true);
                this->hasExcludedObjects=true;
            }
        }
        // This still can be a bone object that is not used by any skin.
        // We dont want to export a sceneObject for that!
        if (obj->ClassID()==BONE_OBJ_CLASSID)
            allBonesCache->Set(obj, true);

        // The object can be part of a biped.
        // We dont want to export a sceneObject for that!
        Control* control = node->GetTMController();
        if ( control )
        {
            Class_ID controllerClassId = control->ClassID();
            if (   controllerClassId == BIPSLAVE_CONTROL_CLASS_ID
                || controllerClassId == BIPBODY_CONTROL_CLASS_ID
                || controllerClassId == FOOTPRINT_CLASS_ID
                || controllerClassId == BIPED_CLASS_ID )
                {
                allBonesCache->Set(obj, true);
            }
        }
    }

    // update ProgressBar and call this function on all children of this obj (recursive)
    int i;
    int numChildren = node->NumberOfChildren();
    UpdateProgressBar(MAXAWD_PHASE_PREPROCESS_SCENEGRAPH, (double)numNodesTraversed/(double)numNodesTotal);
    for (i=0; i<numChildren; i++) {
        PreProcessSceneGraph(node->GetChildNode(i), excludeChild, blockSettings);
        RETURN_IF_ERROR;
    }
}

void MaxAWDExporter::ProcessSceneGraph(INode *node, AWDSceneBlock *parent, BlockSettings* blockSettings, bool isCombinedGeo)
{
    Object *obj;
    bool goDeeper;
    bool isCombinedRoot=false;
    AWDSceneBlock *awdParent = NULL;

    // By default, also parse children of this node
    goDeeper = true;

    obj = node->GetObjectRef();

    if (obj) {
    // if the obj can be found in the excluded cache, we can just return (no childs of this will be exported )
        bool isBone=allBonesCache->Get(obj);
        if (isBone)
           return;
        bool isExcluded=allExcludedCache->Get(obj);
        if (isExcluded)
           return;
        else{
            if ((node->Selected()!=0) || (parent!=NULL) || (exportAll)){
                INodeTab nodes;
                SClass_ID sid=obj->SuperClassID();
                Object * thisBaseObj=(Object*)nodesToBaseObjects->Get(node);
                ULONG handle = 0;
                obj->NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
                INode *originalNode = GetCOREInterface()->GetINodeByHandle(handle);
                if(thisBaseObj==NULL){
                    IInstanceMgr::GetInstanceMgr()->GetInstances(*originalNode, nodes);
                    thisBaseObj = originalNode->GetObjectRef();
                    int i=0;
                    for(i=0;i<nodes.Count();i++){
                        nodesToBaseObjects->Set(nodes[i], thisBaseObj);
                    }
                }
                if((thisBaseObj->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (thisBaseObj->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (thisBaseObj->SuperClassID() == DERIVOB_CLASS_ID )){
                    IDerivedObject* thisBaseObj_der = (IDerivedObject*)(thisBaseObj);
                    thisBaseObj = thisBaseObj_der->GetObjRef();
                    while (thisBaseObj!=thisBaseObj->FindBaseObject())
                        thisBaseObj=thisBaseObj->FindBaseObject();
                }
                getBaseObjectAndID(obj, sid);
                //obj = node->GetObjectRef();
                int mtxTime=maxInterface->GetTime();
                Matrix3 mtx = node->GetNodeTM(mtxTime) * Inverse(node->GetParentTM(mtxTime));
                double *mtxData = (double *)malloc(12*sizeof(double));
                SerializeMatrix3(mtx, mtxData);
                bool isExported=false;
                if (sid==CAMERA_CLASS_ID){
                    if ((opts->ExportScene())&&(opts->ExportCameras())){
                        //output_debug_string("Camera");
                        awdParent=(AWDSceneBlock*)ExportCameraAndTextureExporter(node, mtxData, awdParent, blockSettings);
                    }
                    isExported=true;
                    //export camera
                }
                else if (sid==SHAPE_CLASS_ID){
                    isExported=true;
                    //shapes are not supported, so skip  this
                }
                else if (sid==HELPER_CLASS_ID){
                    //do nothing, export as ObjectContainer3D
                }
                else if (sid==MATERIAL_CLASS_ID){
                    isExported=true;
                    //ERROR material should not occur here....
                }
                else if (sid==LIGHT_CLASS_ID){
                    // lights should allready been created, but not included in the scene graph
                    // so we get the light from the cache and insert it into the scen graph
                    if ((opts->ExportScene())&&(opts->IncludeLights())){
                        //output_debug_string("   -->Object will be exported as a Light");
                        AWDLight * awdLight=lightCache->Get(node);
                        if (awdLight==NULL){
                            int test=0;//ERROR SHOULD Always found the light !
                        }
                        else{
                            if (awdLight->get_light_type()==AWD_LIGHT_DIR){
                                //Point3 zaxis=mtx.GetRow(2);
                                //awdLight->set_directionVec(zaxis.x, zaxis.y, zaxis.z);
                                awdLight->set_directionVec(-1*mtxData[3], -1*mtxData[4], -1*mtxData[5]);
                            }
                            awdLight->set_transform(mtxData);
                            if (parent) {
                                parent->add_child(awdLight);
                            }
                            else {
                                awd->add_scene_block(awdLight);
                            }
                            awdParent=awdLight;
                        }
                    }
                    isExported=true;
                }
                else if (true){//sid==GEOMOBJECT_CLASS_ID){
                    AWDAnimator * animatorBlock=NULL;
                    Object* node_bo = node->GetObjectRef();
                    AWDBlockList * autoMethodBlocks=NULL;
                    if((node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == DERIVOB_CLASS_ID )){
                        IDerivedObject* node_der = (IDerivedObject*)(node_bo);
                        node_bo = node_der->GetObjRef();
                        autoMethodBlocks=(AWDBlockList *)autoApplyMethodsToINodeCache->Get(node);

                        if(!isCombinedGeo){
                            //bool combinedReturn=IsCombinedGeom(node);//set to true if combined root
                            //if(combinedReturn==1){
                                //get the blocklist to gather geoms
                            //}
                            // check if this has a combined subgeo modifier applied
                        }
                        animatorBlock=GetAWDAnimatorForObject(node);
                    }
                    MSTR className;
                    // check if this is a AWDSkyBox
                    node_bo->GetClassName(className);
                    char * className_ptr=W2A(className);
                    if ((ATTREQ(className_ptr, "AWDSkyBox"))&&(!isCombinedRoot)) {
                        //output_debug_string("   -->Object will be exported as a SkyBox");
                        char * skyBoxName_ptr=W2A(node->GetName());
                        AWDSkyBox * newSkyBox = new AWDSkyBox(skyBoxName_ptr, strlen(skyBoxName_ptr));
                        Mtl *mtl = node->GetMtl();
                        if (mtl!=NULL){
                            MSTR matClassName;
                            mtl->GetClassName(matClassName);
                            char * classNameCubeTex_ptr=W2A(matClassName);
                            if (ATTREQ(classNameCubeTex_ptr,"AWDCubeMaterial")){
                                AWDCubeTexture* newCubetex = ExportAWDCubeTexure((MultiMtl *)mtl);
                                if (newCubetex!=NULL){
                                    newSkyBox->set_cube_tex(newCubetex);
                                }
                            }
                            free(classNameCubeTex_ptr);
                        }
                        if(newSkyBox->get_cube_tex()==NULL){
                            AWDMessageBlock * newWarning = new AWDMessageBlock(skyBoxName_ptr, "Skybox has no AWDCubetexture assigned. will not be exported.");
                            awd->get_message_blocks()->append(newWarning);
                            newSkyBox->make_invalide();
                        }
                        if (parent) {
                            AWDMessageBlock * newWarning = new AWDMessageBlock(skyBoxName_ptr, "Skybox can not be child of another object. will be exported as root objects");
                            awd->get_message_blocks()->append(newWarning);
                            awd->add_scene_block(newSkyBox);
                        }
                        free(skyBoxName_ptr);
                        awd->add_scene_block(newSkyBox);
                        isExported=true;
                    }
                    else{
                        int time=maxInterface->GetTime();
                        bool hasVertexAnim=vetexAnimNeutralPosecache->hasKey(node);
                        if(hasVertexAnim)
                            time=vetexAnimNeutralPosecache->Get(node)*GetTicksPerFrame();
                        // check if the object contains any triangles/faces. otherwise we can cancel here.
                        TriObject *triObject = (TriObject*)thisBaseObj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));
                        if (triObject!=NULL){
                            Mesh mesh = triObject->mesh;
                            if (mesh.getNumFaces()>0){
                                //output_debug_string("   -->Object is a Geometry");
                                AWDBlockList * matBlocks=NULL;
                                matBlocks=GetMaterialsForMeshInstance(node);
                                if (autoMethodBlocks!=NULL){
                                    if (autoMethodBlocks->get_num_blocks()>0){
                                        if (matBlocks!=NULL){
                                            AWDMaterial * awdMat=NULL;
                                            AWDBlockIterator *itMat;
                                            itMat = new AWDBlockIterator(matBlocks);
                                            while ((awdMat = (AWDMaterial*)itMat->next()) != NULL) {
                                                AWDBlock * awdFX=NULL;
                                                AWDBlockIterator *itFX;
                                                itFX = new AWDBlockIterator(autoMethodBlocks);
                                                while ((awdFX = (AWDBlock*)itFX->next()) != NULL) {
                                                    awdMat->get_effectMethods()->append(awdFX);
                                                }
                                                delete itFX;
                                            }
                                            delete itMat;
                                        }
                                    }
                                }
                                if (opts->ExportGeometry()) {
                                    // if the mesh has a AWD-animator-modifier applied, we do not consider exporing it as primitive
                                    // if it has no AWD-animator-modifier applied, we check for AWD-AnimSources, that are set to auto-create AWDAnimators

                                    AWDTriGeom *awdGeom=NULL;
                                    AWDPrimitive *awdPrimGeom=NULL;
                                    // check if a Vertex-AnimSource-Modifier is applied to mesh (with settings: simpleMode=true)
                                    // if one is found, we create a AWDAnimator and Animationset for this.
                                    if (animatorBlock==NULL){
                                        if(opts->ExportVertexAnim()){
                                            //output_debug_string("      -->Try to auto create Animation-Setup for VertexAnimation");
                                            animatorBlock=AutoCreateAnimatorForVertexAnim(node);
                                        }
                                        if (animatorBlock==NULL){
                                            if(opts->ExportSkelAnim()){
                                                //output_debug_string("      -->Try to auto create Animation-Setup for SkeletonAnimation");
                                                animatorBlock=AutoCreateAnimatorForSkeleton(node);
                                            }
                                            // if still no AWD-animator has been created for this mesh, we check if we can export it as primitive
                                            if ((animatorBlock==NULL)&&((matBlocks==NULL)||((matBlocks!=NULL)&&(matBlocks->get_num_blocks()==1)))){
                                                if (opts->ExportPrimitives()){
                                                    //output_debug_string("      -->Check if Object can be exported as Primitve");
                                                    char *bname = W2A(node->GetName());
                                                    char *primName_ptr = (char*)malloc(strlen(bname)+6);
                                                    strcpy(primName_ptr, bname);
                                                    strcat(primName_ptr, "_prim");
                                                    free(bname);
                                                    awdPrimGeom=ExportPrimitiveGeom(thisBaseObj, primName_ptr);
                                                    free(primName_ptr);
                                                }
                                            }
                                            if ((animatorBlock==NULL)&&(matBlocks!=NULL)){
                                                //output_debug_string("      -->Try to auto create Animation-Setup for SkeletonAnimation");
                                                animatorBlock=AutoCreateAnimatorForUV(node, matBlocks);
                                                if(animatorBlock==NULL)
                                                    animatorBlock=AutoCreateAnimatorForUV(originalNode, matBlocks);
                                            }
                                        }
                                    }
                                    if (awdPrimGeom==NULL){
                                        awdGeom = (AWDTriGeom *)geometryCache->Get(thisBaseObj);
                                        if (awdGeom == NULL){
                                            //output_debug_string("      -->Create a TriGeom for the Object");
                                            char *bname = W2A(node->GetName());
                                            char *triGeoName_ptr = (char*)malloc(strlen(bname)+6);
                                            strcpy(triGeoName_ptr, bname);
                                            strcat(triGeoName_ptr, "_geom");
                                            free(bname);
                                            awdGeom = new AWDTriGeom(triGeoName_ptr, strlen(triGeoName_ptr));
                                            free(triGeoName_ptr);
                                            geometryCache->Set(thisBaseObj, awdGeom);
                                            awd->add_mesh_data(awdGeom);
                                            INodeToGeoBlockCache->Set(awdGeom, node);
                                        }
                                        if (animatorBlock!=NULL){
                                            if(animatorBlock->get_anim_type()==ANIMTYPEVERTEX){
                                                AWDAnimationSet * animSet=animatorBlock->get_animationSet();
                                                if(animSet!=NULL){
                                                    if(animSet->get_sourcePreID()!=NULL){
                                                        INode* animNode=(INode *) sourceObjsIDsCache->Get(animSet->get_sourcePreID());
                                                        if (animNode==node){
                                                            animSet->set_target_geom(awdGeom);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        if (matBlocks != NULL){
                                            if (matBlocks->get_num_blocks()>1){
                                                awdGeom->set_split_faces(true);
                                            }
                                        }
                                    }
                                    int castShadows=node->CastShadows();
                                    AWDLightPicker * lightPicker=NULL;
                                    if ((opts->ExportScene())&&(opts->ExportMaterials())&&(matBlocks != NULL)) {
                                        //output_debug_string("      -->get the Lightpicker for the materials applied to the Object");
                                        lightPicker=lightCache->GetLightPickerForMesh(node);
                                        if(lightPicker!=NULL)
                                            awd->add_light_picker_block(lightPicker);
                                    }
                                    AWDMeshInst *awdMesh=NULL;
                                    if ((awdPrimGeom==NULL)&&(awdGeom==NULL)){
                                        //output_debug_string("print ('-->empty mesh')");
                                    }

                                    char * meshName_ptr=W2A(node->GetName());
                                    if (awdPrimGeom!=NULL){
                                        //output_debug_string("      -->Create a MeshInstance for the Primitive");
                                        mtxData[10]+=awdPrimGeom->get_Yoffset();
                                        awdMesh = new AWDMeshInst(meshName_ptr, strlen(meshName_ptr), awdPrimGeom, mtxData);
                                        if(castShadows==0){
                                            awdMesh->add_bool_property(5, false, true);
                                        }
                                        if (matBlocks!=NULL){
                                            AWDMaterial * newPrimMat=(AWDMaterial *)matBlocks->getByIndex(0);
                                            if(lightPicker!=NULL){
                                                AWDMaterial * newPrimMatLP=newPrimMat->get_unique_material(lightPicker, NULL, NULL);
                                                if(opts->SetMultiPass()){
                                                    // multipass using the number of lights, that the lightpicker uses
                                                    newPrimMatLP->set_multiPass(false);
                                                    if (lightPicker->get_lights()->get_num_blocks()>4)
                                                        newPrimMatLP->set_multiPass(true);
                                                }
                                                if ((lightPicker->get_lights()->get_num_blocks()>4)&&(!newPrimMatLP->get_multiPass())){
                                                    AWDMessageBlock * newWarning = new AWDMessageBlock(newPrimMatLP->get_name(), "AWDMaterial has more than 3 lights assigned, but is set to singlepass. this might cause problems on render.");
                                                    awd->get_message_blocks()->append(newWarning);
                                                }
                                                if(opts->IncludeShadows()){
                                                    if(newPrimMatLP->get_shadowMethod()!=NULL){
                                                        bool shadowOK=lightPicker->check_shadowMethod((AWDShadowMethod *)(newPrimMatLP->get_shadowMethod()));
                                                        if(!shadowOK){
                                                            AWDMessageBlock * newWarning = new AWDMessageBlock(newPrimMatLP->get_name(), "Could not find the ShadowMethod thats applied to the material on one of the lights that it assigned to the material.");
                                                            awd->get_message_blocks()->append(newWarning);
                                                            newPrimMatLP->set_shadowMethod(NULL);
                                                        }
                                                        }
                                                    if(newPrimMatLP->get_shadowMethod()==NULL){
                                                        newPrimMatLP->set_shadowMethod(lightPicker->get_shadowMethod());
                                                    }
                                                }
                                                awdMesh->add_material(newPrimMatLP);
                                            }
                                            else{
                                                awdMesh->add_material(newPrimMat);
                                            }
                                            delete matBlocks;
                                        }
                                    }
                                    else if (awdGeom!=NULL){
                                        //output_debug_string("      -->Create a MeshInstance for the TriGeom");
                                        awdMesh = new AWDMeshInst(meshName_ptr, strlen(meshName_ptr), awdGeom, mtxData);
                                        if(castShadows==0){
                                            awdMesh->add_bool_property(5, false, true);
                                        }
                                        if (awdGeom != NULL){
                                            if (matBlocks!=NULL){
                                                if (matBlocks->get_num_blocks()==1)
                                                    awdMesh->set_defaultMat(matBlocks->getByIndex(0));
                                                else{
                                                    awdMesh->set_defaultMat(getColorMatForObject(node, true));
                                                }
                                                awdMesh->set_pre_materials(matBlocks);
                                            }
                                            awdGeom->get_mesh_instance_list()->append(awdMesh);
                                        }
                                        if (awdMesh) {
                                            if (lightPicker!=NULL)
                                                awdMesh->set_lightPicker(lightPicker);
                                            if (animatorBlock!=NULL)
                                                animatorBlock->add_target(awdMesh);
                                        }
                                    }
                                    free(meshName_ptr);
                                    if (awdMesh!=NULL){
                                        if(animatorBlock!=NULL)
                                            awdMesh->set_animator(animatorBlock);
                                        ExportUserAttributesForNode(node, thisBaseObj, awdMesh);
                                        //output_debug_string("      -->Place object into scenegraph");
                                        if (parent)
                                            parent->add_child(awdMesh);
                                        else
                                            awd->add_scene_block(awdMesh);
                                        awdParent = awdMesh;
                                    }
                                }
                                isExported=true;
                            }
                            else{
                            }
                        }
                    }
                    free(className_ptr);
                }
                if (!isExported){
                    //output_debug_string("   -->NO OBJECT Created - create container");
                    char * containerName_ptr=W2A(node->GetName());
                    awdParent=new AWDContainer(containerName_ptr, strlen(containerName_ptr), mtxData);
                    ExportUserAttributesForNode(node, thisBaseObj, awdParent);
                    free(containerName_ptr);
                    if (parent)
                        parent->add_child(awdParent);
                    else
                        awd->add_scene_block(awdParent);
                }
                free(mtxData);
            }
            else
            {
                // TODO: ERROR because no (3dsmax)obj was found (?)
            }
        }
    }
    numNodesTraversed++;

    if (goDeeper) {
        //output_debug_string("");
        //output_debug_string("   -->PARSE CHILDREN:");
        int i;
        int numChildren = node->NumberOfChildren();
        // Update progress bar before recursing
        UpdateProgressBar(MAXAWD_PHASE_EXPORT_SCENEGRAPH, (double)numNodesTraversed/(double)numNodesTotal);

        for (i=0; i<numChildren; i++) {
            ProcessSceneGraph(node->GetChildNode(i), awdParent, blockSettings, isCombinedGeo);
            RETURN_IF_ERROR;
        }
    }
    else {
        // No need to traverse this branch further. Count all
        // descendants as traversed and update progress bar.
        numNodesTraversed += CalcNumDescendants(node);
        UpdateProgressBar(MAXAWD_PHASE_EXPORT_SCENEGRAPH, (double)numNodesTraversed/(double)numNodesTotal);
    }
}