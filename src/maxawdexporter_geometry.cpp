#include "maxawdexporter.h"

void MaxAWDExporter::ProcessGeoBlocks()
{
    AWDTriGeom *geoBlock;
    AWDBlockIterator *it;
    int proccessed=0;
    it = new AWDBlockIterator(awd->get_mesh_data_blocks());
    UpdateProgressBar(MAXAWD_PHASE_PROCESS_GEOMETRY, (double)proccessed/(double)awd->get_mesh_data_blocks()->get_num_blocks());
    INodeTab lNodes;
    while ((geoBlock = (AWDTriGeom * ) it->next()) != NULL){
        INode * node = (INode *)INodeToGeoBlockCache->Get(geoBlock);
        if (node==NULL){
            AWDMessageBlock * newWarning = new AWDMessageBlock(geoBlock->get_name(), "ERROR: Could not find the INode for this AWDGeometry.");
            awd->get_message_blocks()->append(newWarning);
            return;
        }
        lNodes.AppendNode( node );
    }

    IGameScene* _pIgame = NULL;
    _pIgame = GetIGameInterface();
    _pIgame->InitialiseIGame( lNodes );
    it->reset();
    while ((geoBlock = (AWDTriGeom * ) it->next()) != NULL){
        INode * node = (INode *)INodeToGeoBlockCache->Get(geoBlock);
        if (node==NULL){
            AWDMessageBlock * newWarning = new AWDMessageBlock(geoBlock->get_name(), "ERROR: Could not find the INode for this AWDGeometry.");
            awd->get_message_blocks()->append(newWarning);
        }
        else{
            int exportThis=false;
            IGameObject * gobj = NULL;
            IGameMesh * igame_mesh = NULL;
            gobj = GetIGameInterface()->GetIGameNode(node)->GetIGameObject();
            if(gobj->GetIGameType()==IGameObject::IGAME_MESH){
                igame_mesh = (IGameMesh*)gobj;
                if (igame_mesh!=NULL){
                    igame_mesh->InitializeData();
                    if(igame_mesh->GetNumberOfFaces()>0){
                        exportThis=true;
                    }
                }
            }
            if (exportThis){
                Object *obj;
                obj = node->GetObjectRef();
                int skinIdx;
                ObjectState os;
                IDerivedObject *derivedObject = NULL;
                skinIdx = IndexOfSkinMod(node->GetObjectRef(), &derivedObject);
                if (skinIdx >= 0) {
                    // Flatten all modifiers up to but not including
                    // the skin modifier.
                    // to do: get the correct time for the neutral-pose
                    os = derivedObject->Eval(0, skinIdx + 1);
                }
                else {
                    // Flatten entire modifier stack
                    // to do: get the correct time for the neutral-pose
                    os = node->EvalWorldState(maxInterface->GetTime());
                }
                obj = os.obj;
                ISkin *skin = NULL;
                if (derivedObject != NULL && skinIdx >= 0) {
                    Modifier *mod = derivedObject->GetModifier(skinIdx);
                    skin = (ISkin *)mod->GetInterface(I_SKIN);
                }
                ExportTriGeom(geoBlock,obj,node,skin, igame_mesh);
                RETURN_IF_ERROR;
            }
        }
        proccessed++;
        UpdateProgressBar(MAXAWD_PHASE_PROCESS_GEOMETRY, (double)proccessed/(double)awd->get_mesh_data_blocks()->get_num_blocks());
    }
    delete it;
    _pIgame->ReleaseIGame();
}

void MaxAWDExporter::ExportTriGeom(AWDTriGeom *awdGeom, Object *obj, INode *node, ISkin *skin, IGameMesh * igame_mesh)
{
    if (awdGeom != NULL) {
        if (awdGeom->get_is_created())
            return;
        else {
            awdGeom->set_is_created(true);

            // Extract skinning information (returns number of joints per vertex),
            // and writes the weights and joints array according to the jpv
            int jpv=0;
            awd_float64 *weights=NULL;
            awd_uint32 *joints=NULL;
            jpv = ExportSkin(node, skin, &weights, &joints);
            // Calculate offset matrix from the object TM (which includes geometry offset)
            // this will be used to transform all vertices into node space.
            int time=maxInterface->GetTime();

            // get the neutral pose time from the vertex (using a dedcated cache for this)
            bool hasVertexAnim=vetexAnimNeutralPosecache->hasKey(node);
            if(hasVertexAnim)
                time=vetexAnimNeutralPosecache->Get(node)*GetTicksPerFrame();
            else{
                // get the neutral pose time from the skeleton, if any is used
                if (opts->ExportSkin() && skin && jpv>0) {
                    SkeletonCacheItem *skel = skeletonCache->GetFromBone(skin->GetBone(0));
                    time=skel->awdSkel->get_neutralPose();
                }
            }

            Matrix3 offsMtx = node->GetObjectTM(time) * Inverse(node->GetNodeTM(time));
            bool isInSkinPoseMode=false;
            double *mtxData = (double *)malloc(12*sizeof(double));
            SerializeMatrix3(offsMtx, mtxData);
            /*if (skin && jpv>0) {
                // if the mesh is not in "Skin Pose Mode",
                // we add the bind-matrix to the offset matrix (so that all points are moved accoridingly)
                // and later we set the transform matrix of the mesh to the identity-matrix (no transform)
                ISkinPose * skinPose;
                skinPose=skinPose->GetISkinPose(*node);
                //isInSkinPoseMode=skinPose->SkinPoseMode();
                Matrix3 mtx;
                mtx.IdentityMatrix();
                if (!isInSkinPoseMode){
                    Matrix3 bm;
                    bm.IdentityMatrix();
                    //skin->GetSkinInitTM(node, bm, true);
                    //offsMtx *= bm;
                    //SerializeMatrix3(mtx, mtxData);
                }
                skinPose=NULL;
            }*/

            ObjectState os;
            // Flatten entire modifier stack
            os = node->EvalWorldState(time);
            obj = os.obj;
            // its allready been taken care that the correct obj is submitted to this function, so we can directly convert to TriObject (?)
            TriObject *triObject = (TriObject*)obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));

            Mesh mesh = triObject->mesh;
            int numTris = mesh.getNumFaces();
            int numVerts = mesh.getNumVerts();
            awdGeom->set_originalPointCnt(numVerts);

            // This could happen for example with splines (No!, this should never happen, because we check for this earlier (?))
            if (numTris==0)
                return;
            /*
            //TODO: optional reorder exported faces, so that quads are stored first
            //we than can store one uint32 as offset into the triangle-list, to reconstruct quads on import

            int * quadDic=(int *)malloc(sizeof(int) * numTris);
            int * isExportedDic=(int *)malloc(sizeof(int) * numTris);
            int cnt1=0;
            for(cnt1=0;cnt1<numTris;cnt1++){
                quadDic[cnt1]=0;
                isExportedDic[cnt1]=0;
            }
            PolyObject *polyObject = (PolyObject*)obj->ConvertToType(time, Class_ID(POLYOBJ_CLASS_ID, 0));
            if (polyObject!=NULL){
                MNMesh mnMesh = polyObject->GetMesh();
                int numFaces = mnMesh.FNum();
                int numtri = mnMesh.TriNum();
                int idxCnt=0;
                for(idxCnt=0;idxCnt<numFaces;idxCnt++){
                    MNFace thisface=mnMesh.f[idxCnt];
                    if(thisface.TriNum()==2){
                        Tab<int> intList;
                        thisface.GetTriangles(intList);
                        //quadDic[intList[0]]=intList[1];
                        //quadDic[intList[1]]=intList[0];
                    }
                }
                //if (polyObject!=obj)
                //    polyObject->DeleteMe();
            }
            */
            bool force_split=opts->SplitByMatID();// if true, the GeomUtils will create a SubGeo for each MaterialID used by a face, no matter if they share materials or not
            bool useUV=opts->ExportUVs();// TODO: check if uvs exists
            bool useSecUVs=useUV; // TODO: check if second UVs exists (and are requested)
            bool useNormals=opts->ExportNormals();

            // ATTENTION:
            // we have collected all meshintsances that are using this geometry.
            // but some material-settings ( UV / SecondUV / explode) can force us to create multiple geometries...

            // the IGAmeMesh gives acces to some handy functions for exporting meshes
            // we still need to use the mesh from the standart api, to have access to the correct UV (?)

            MeshNormalSpec *specificNormals = NULL;
            if(igame_mesh!=NULL){
                int numTrisGameMesh = igame_mesh->GetNumberOfFaces();
                if (numTrisGameMesh!=numTris){
                    return; //ERROR: faceCount of game-mesh is not facecount of api-mesh - should not happen
                }
                AWDBlockList * meshInstanceList = awdGeom->get_mesh_instance_list();
                if (meshInstanceList==NULL){
                    return; //ERROR: faceCount of game-mesh is not facecount of api-mesh - should not happen
                }
                int numMeshInstances=meshInstanceList->get_num_blocks();
                if ((meshInstanceList!=NULL)&&(numMeshInstances==0)){
                    return; //ERROR: faceCount of game-mesh is not facecount of api-mesh - should not happen
                }
                // check if the first UVChannel is available for this mesh ( numTVFaces must be equal to numTris)
                if (useUV) {
                    try{
                        if (mesh.mapSupport(1)){
                            MeshMap * mesh_map;
                            mesh_map = &(mesh.Map(1));
                            int numTrisMap = mesh_map->getNumFaces();
                            if (numTrisMap!=numTris){
                                useUV=false;
                                useSecUVs=false;
                            }
                        }
                        else{
                            useUV=false;
                            useSecUVs=false;
                        }
                    }
                    catch(...){
                        useUV=false;
                        useSecUVs=false;
                    }
                }
                // check if any normals are available for the mesh
                if (useNormals) {
                    mesh.SpecifyNormals();
                    specificNormals = mesh.GetSpecifiedNormals();
                    int  specificNormalCount = specificNormals->GetNumNormals();// for me, this is allways been 0
                    if (specificNormalCount==0){
                        specificNormals=NULL;
                        if(igame_mesh->IsObjectSkinned()){
                            igame_mesh->InitializeBinormalData();
                            igame_mesh->InitializeData();
                        }
                       // else{
                            int numNorms = igame_mesh->GetNumberOfNormals();
                            if (numNorms==0){
                                useNormals=false;
                        //    }
                        }
                    }
                }

                AWD_field_type precision_geo=AWD_FIELD_FLOAT32;
                if (opts->StorageGeometry()==1)
                    precision_geo=AWD_FIELD_FLOAT64;
                AWDGeomUtil * geomUtil=new AWDGeomUtil(awdGeom->get_split_faces(), force_split, useUV, useSecUVs, useNormals, 0.0, jpv, precision_geo);
                // create a list of GUGeom for each Mesh instance.
                // before collecting the actual geom-data,
                // we will reduce the number of GUGeoms to the minimum needed to display all mesh-instances correctly
                geomUtil->createPreGeometries(meshInstanceList);
                Tab<int> MatIDList=igame_mesh->GetActiveMatIDs();
                int matIDCnt;
                Tab<FaceEx *> facelist;
                // for each submesh do:
                for (matIDCnt=0; matIDCnt< MatIDList.Count(); matIDCnt++){
                    facelist=igame_mesh->GetFacesFromMatID(MatIDList[matIDCnt]);
                    int idx;
                    int faceCnt=facelist.Count();
                    if (faceCnt>0){
                        // if the submesh will be used (if the matID is used by any face):
                        AWDBlockList * subMaterialList = new AWDBlockList();
                        int meshInstCnt=0;
                        // for each mesh instance, apply the material
                        for (meshInstCnt=0;meshInstCnt<numMeshInstances; meshInstCnt++){
                            AWDMeshInst * awdMesh=(AWDMeshInst *)meshInstanceList->getByIndex(meshInstCnt);
                            if (awdMesh==NULL){
                                return;
                            }
                            else{
                                AWDBlockList * preMaterials = awdMesh->get_pre_materials();
                                if (preMaterials==NULL){
                                    return;
                                }
                                else{
                                    bool createDefault = false;
                                    AWDMaterial * thisMatBlock = (AWDMaterial *)preMaterials->getByIndex(MatIDList[matIDCnt]);
                                    if (thisMatBlock==NULL){
                                        thisMatBlock=(AWDMaterial *)awdMesh->get_defaultMat();
                                    }
                                    if (useUV){
                                        if (thisMatBlock->get_mappingChannel()>0)
                                            thisMatBlock->set_mappingChannel(checkIfUVMapExists(mesh, numTris, thisMatBlock->get_mappingChannel()));
                                    }
                                    if (useSecUVs){
                                        if (thisMatBlock->get_secondMappingChannel()>0)
                                            thisMatBlock->set_secondMappingChannel(checkIfUVMapExists(mesh, numTris, thisMatBlock->get_secondMappingChannel()));
                                    }
                                    subMaterialList->force_append(thisMatBlock);
                                }
                            }
                        }
                        geomUtil->add_new_sub_geo_to_preGUgeoms(subMaterialList, MatIDList[matIDCnt]);
                        delete subMaterialList;
                    }
                }
                geomUtil->createGeometries();

                for (matIDCnt=0; matIDCnt<MatIDList.Count(); matIDCnt++){
                    facelist=igame_mesh->GetFacesFromMatID(MatIDList[matIDCnt]);
                    int idx;
                    int faceCnt=facelist.Count();
                    if (faceCnt>0){
                        int numGeoms=geomUtil->get_geoList()->get_num_blocks();
                        int geoCnt;
                        for(geoCnt=0; geoCnt<numGeoms; geoCnt++){
                            GUGeo * thisGUGeo = geomUtil->get_geoList()->get_by_idx(geoCnt);
                            // get the uvs using the index stored in the geomUtil
                            // get the secondUV using the index stored in the geomUtil
                            // get the explode (normals) using the index stored in the geomUtil

                            // ATTENTION: its not a Subgeo but a SubgeoGroup, so it contain several Subgeos if the face or vert lists are to big for one subgeo
                            int thisSubGeoIdx=geomUtil->getSubGeoIdxForMatIdx(MatIDList[matIDCnt]);
                            GUSubGeoGroup *thisSubGeoGroup = thisGUGeo->get_subGeomList()->get_by_idx(thisSubGeoIdx);
                            if (thisSubGeoGroup==NULL){
                                return;
                            }
                            AWDMaterial * thisAWDMat=(AWDMaterial *)thisSubGeoGroup->materials->getByIndex(0);
                            bool explode=thisAWDMat->get_is_faceted();

                            MeshMap * mainUVMeshMap=NULL;
                            MeshMap * secondUVMeshMap=NULL;

                            if (thisSubGeoGroup->include_uv){
                                if (thisAWDMat->get_mappingChannel()>0)
                                    mainUVMeshMap = &(mesh.Map(thisAWDMat->get_mappingChannel()));
                            }
                            if (thisSubGeoGroup->include_suv){
                                if (thisAWDMat->get_secondMappingChannel()>0)
                                    secondUVMeshMap = &(mesh.Map(thisAWDMat->get_secondMappingChannel()));
                            }
                            // for each face in the list do:
                            bool hasMultipleUV=false;
                            for (idx=0;idx<faceCnt;idx++){
                                /*if(isExportedDic[idx]==0){
                                    isExportedDic[idx]=1;
                                    if(quadDic[idx]>0){
                                        int yes=0;
                                    }*/
                                    // this will create a new SubGeo inside the SubgeoGroup, if the limits are reached
                                thisSubGeoGroup->check_limits();
                                // create a new vert
                                FaceEx * f=facelist[idx];
                                int apiFaceIdx=f->meshFaceIndex;
                                TVFace tvface;
                                TVFace tvFaceSecond;
                                Face face = mesh.faces[apiFaceIdx];
                                DWORD *inds = face.getAllVerts();
                                if (thisSubGeoGroup->include_uv){
                                    if (mainUVMeshMap!=NULL)
                                        tvface = mainUVMeshMap->tf[apiFaceIdx];
                                    else
                                        tvface = mesh.tvFace[apiFaceIdx];
                                }
                                if (thisSubGeoGroup->include_suv){
                                    if (secondUVMeshMap!=NULL)
                                        tvFaceSecond = secondUVMeshMap->tf[apiFaceIdx];
                                    else
                                        tvFaceSecond = mesh.tvFace[apiFaceIdx];
                                }
                                Point3 faceNormal;
                                if (geomUtil->include_normals) {
                                    // if we want to export normals, but no normals was read, than we need to calculate ourself,
                                    // using the face-normal for the angle-calulation
                                    if ((igame_mesh==NULL && specificNormals==NULL)||(explode)){
                                        // faceNormal = mesh.getFaceNormal(t); // this crashes 3dsmax (why?), so calulate the facenormal manually:
                                            Point3 v0, v1, v2;
                                            Tab<Point3> fnorms;
                                            v0 = mesh.getVert(face.getVert(0));
                                            v1 = mesh.getVert(face.getVert(1));
                                            v2 = mesh.getVert(face.getVert(2));
                                            faceNormal = (v1-v0)^(v2-v1);
                                            faceNormal = Normalize(faceNormal);
                                    }
                                }
                                int v;
                                for (v=2; v>=0; v--) {
                                    int vIdx = face.getVert(v);
                                    Point3 vtx = offsMtx * mesh.getVert(vIdx);

                                    vdata *vd = (vdata *)malloc(sizeof(vdata));
                                    vd->orig_idx = vIdx;
                                    vd->x = vtx.x;
                                    vd->y = vtx.z;
                                    vd->z = vtx.y;
                                    // Might not have UV coords
                                    if (geomUtil->include_uv) {
                                        int tvIdx;
                                        Point3 tvtx;
                                        Point3 stvtx;
                                        if (mainUVMeshMap!=NULL)
                                            tvtx=mainUVMeshMap->tv[tvface.t[v]];
                                        else{
                                            tvIdx = tvface.getTVert(v);
                                            tvtx = mesh.getTVert(tvIdx);
                                        }

                                        if (secondUVMeshMap!=NULL)
                                            stvtx=secondUVMeshMap->tv[tvFaceSecond.t[v]];
                                        else{
                                            tvIdx = tvface.getTVert(v);
                                            stvtx = mesh.getTVert(tvIdx);
                                        }
                                        vd->u = tvtx.x;
                                        vd->v = 1.0-tvtx.y;
                                        vd->su = stvtx.x;
                                        vd->sv = 1.0-stvtx.y;
                                    }

                                    if (geomUtil->include_normals) {
                                        Point3 normal;
                                        // if specific vertex-normals was found, we use it, if the subgeo is not set to explode
                                        if ((specificNormals!=NULL) && (!explode)){
                                            normal = specificNormals->GetNormal(apiFaceIdx, v);
                                        }
                                        // else if a (not specific) vertex-normals was found (on the igame-mesh), we use it, if the subgeo is not set to explode
                                        else if ((igame_mesh!=NULL) && (!explode)){
                                            igame_mesh->GetNormal(f->norm[v], normal, true);
                                        }
                                        // else: since we still want normals exported, we use the face-normal (using facenormal with threshold of 0 will explode the mesh
                                        else{
                                            // i dont think this should really get executed anymore (since the igame-object allways should give access to the normals)
                                            normal=faceNormal;
                                        }
                                        // if the object is skinned, we get the global normals
                                        if (jpv>0){
                                           // if (normal)
                                            //    normal=offsMtx*normal;
                                        }

                                        vd->nx = normal.x;
                                        vd->ny = normal.z;
                                        vd->nz = normal.y;
                                    }

                                    // If there is skinning information, copy it from the weight
                                    // and joint index arrays returned by ExportSkin() above.
                                    vd->num_bindings = jpv;
                                    if (jpv > 0) {
                                        vd->weights = (awd_float64*)malloc(jpv*sizeof(awd_float64));
                                        vd->joints = (awd_uint32*)malloc(jpv*sizeof(awd_uint32));

                                        int memoffs = jpv*vIdx;
                                        memcpy(vd->weights, weights+memoffs, jpv*sizeof(awd_float64));
                                        memcpy(vd->joints, joints+memoffs, jpv*sizeof(awd_uint32));
                                    }

                                    vd->force_hard = false;

                                    // add the new vertex to the subgeo
                                    thisSubGeoGroup->append_vdata(vd);
                                }
                            }
                        }
                    }
                }
                AWDBlockList * returned_geoms =  geomUtil->build_geom(awdGeom);

                AWDBlockIterator * it=NULL;
                AWDMeshInst * block;
                int maxCount=0;
                int geomCnt=0;
                for (geomCnt=0; geomCnt<returned_geoms->get_num_blocks();geomCnt++){
                    AWDTriGeom * thisAWDGeom=(AWDTriGeom *)returned_geoms->getByIndex(geomCnt);
                    it = new AWDBlockIterator(thisAWDGeom->get_mesh_instance_list());
                    AWDSubGeom *sub;
                    sub = thisAWDGeom->get_first_sub();
                    while (sub) {
                        AWDBlockList *subGeoGroupMatList=sub->get_materials();
                        int thisIdx=0;
                        it->reset();
                        while ((block = (AWDMeshInst*)it->next()) != NULL) {
                            block->set_geom(thisAWDGeom);
                            //if (!isInSkinPoseMode){
                            //    block->set_transform(mtxData);
                            //}
                            AWDMaterial * thisMat=(AWDMaterial *)subGeoGroupMatList->getByIndex(thisIdx);
                            if (thisMat==NULL){
                                int test=0;
                                //ERROR - this should never happen
                            }
                            else{
                                AWDLightPicker * lightPicker=(AWDLightPicker *)block->get_lightPicker();
                                AWDBlock * thisAnimator=(AWDBlock *)block->get_animator();
                                if ((lightPicker!=NULL)||(thisAnimator!=NULL)){
                                    thisMat=thisMat->get_unique_material(lightPicker, thisAnimator, NULL);
                                    if(lightPicker!=NULL){
                                        if(opts->SetMultiPass()){
                                            // multipass using the number of lights, that the lightpicker uses
                                            if (lightPicker->get_lights()->get_num_blocks()>4){
                                                thisMat->set_multiPass(true);
                                            }
                                            else{
                                                thisMat->set_multiPass(false);
                                            }
                                        }
                                        if ((lightPicker->get_lights()->get_num_blocks()>4)&&(!thisMat->get_multiPass())){
                                            AWDMessageBlock * newWarning = new AWDMessageBlock(thisMat->get_name(), "AWDMaterial has more than 4 lights assigned, but is set to singlepass. this will cause problems on render.");
                                            awd->get_message_blocks()->append(newWarning);
                                        }
                                        if(opts->IncludeShadows()){
                                            if(thisMat->get_shadowMethod()!=NULL){
                                                bool shadowOK=lightPicker->check_shadowMethod((AWDShadowMethod *)(thisMat->get_shadowMethod()));
                                                if(!shadowOK){
                                                    AWDMessageBlock * newWarning = new AWDMessageBlock(thisMat->get_name(), "Could not find the ShadowMethod thats applied to the material on one of the lights that it assigned to the material.");
                                                    awd->get_message_blocks()->append(newWarning);
                                                    thisMat->set_shadowMethod(NULL);
                                                }
                                                }
                                            if(thisMat->get_shadowMethod()==NULL){
                                                thisMat->set_shadowMethod(lightPicker->get_shadowMethod());
                                            }
                                        }
                                    }
                                }
                                block->add_material((AWDMaterial*)thisMat);
                            }
                            thisIdx++;
                        }
                        sub = sub->next;
                    }
                    delete it;
                }
                delete returned_geoms;
                // If conversion created a new object, dispose it
                if (triObject != obj)
                    triObject->DeleteMe();
                delete geomUtil;
            }
            else{
            }
            //free(quadDic);
            //free(isExportedDic);
            free(mtxData);
            if (weights!=NULL)
                free(weights);
            if (joints!=NULL)
                free(joints);
        }
    }
    return;
}

int MaxAWDExporter::checkIfUVMapExists(Mesh mesh, int numTris, int mapIdx){
    int numUVMaps = mesh.getNumMaps();
    int t=0;
    if (numUVMaps>2){
        if (numUVMaps>=mapIdx){
            if (mesh.mapSupport(mapIdx)){
                MeshMap * mesh_map;
                mesh_map = &(mesh.Map(mapIdx));
                int numTrisMap = mesh_map->getNumFaces();
                if (numTrisMap==numTris){
                    return mapIdx;
                }
            }
        }
    }
    return 0;
}

int MaxAWDExporter::IsCombinedGeom(INode *node){
    int isCombinedGeom=0;
    BaseObject* node_bo = node->GetObjectRef();
    if((node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == DERIVOB_CLASS_ID ))
    {
        IDerivedObject* node_der = (IDerivedObject*)(node_bo);
        node_bo = node_der->GetObjRef();
        if (node_der!=NULL){
            int nMods = node_der->NumModifiers();
            for (int m = 0; m<nMods; m++){
                Modifier* node_mod = node_der->GetModifier(m);
                //DebugPrint("node_mod.IsEnabled() = "+node_mod->IsEnabled());
                //MSTR name=node_mod->GetName();
                MSTR className;
                node_mod->GetClassName(className);
                char * className_ptr=W2A(className);
                if (ATTREQ(className_ptr,"AWDCombinedGeom") ){
                    AWDBlockList * newGeoList = (AWDBlockList *)combinedGeosCache->Get(node_mod);
                    if (newGeoList==NULL){
                        newGeoList=new AWDBlockList();
                        combinedGeosCache->Set(node_mod,newGeoList);
                        free(className_ptr);
                        return 1;
                    }
                    free(className_ptr);
                    return 2;
                }
                free(className_ptr);
            }
        }
    }

    return 0;
}