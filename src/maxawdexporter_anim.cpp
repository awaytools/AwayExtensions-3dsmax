#include "maxawdexporter.h"
#include "platform.h"

void MaxAWDExporter::ExportAnimations(BlockSettings* blockSettings){
    AWDAnimator *block;
    AWDBlockIterator *it;
    AWDBlockList * animatorBlocks = (AWDBlockList *)awd->get_animator_blocks();
    if (animatorBlocks!=NULL){
        if (animatorBlocks->get_num_blocks()>0){
            int animCnt=0;
            it = new AWDBlockIterator(animatorBlocks);
            while ((block = (AWDAnimator * ) it->next()) != NULL) {
                AWDAnimationSet * animSet = block->get_animationSet();
                if (animSet!=NULL){
                    ExportAWDAnimSet(animSet, block->get_targets(), blockSettings);
                }
                animCnt++;
                UpdateProgressBar(MAXAWD_PHASE_PROCESS_ANIMATIONS, (double)animCnt/(double)animatorBlocks->get_num_blocks());
            }
            delete it;
        }
    }
    AWDAnimationSet *animSetBlock;
    AWDBlockList * animSetBlocks = (AWDBlockList *)awd->get_amin_set_blocks();
    if (animSetBlocks!=NULL){
        if (animSetBlocks->get_num_blocks()>0){
            it = new AWDBlockIterator(animSetBlocks);
            while ((animSetBlock = (AWDAnimationSet * ) it->next()) != NULL) {
                ExportAWDAnimSet(animSetBlock, NULL, blockSettings);
            }
            delete it;
        }
    }
}

void MaxAWDExporter::ReadAWDSkeletonMod(Modifier *node_mod, INode * node){
    int num_params = node_mod->NumParamBlocks();
    char * skeletonMod_ptr=NULL;
    int p=0;
    int numBlockparams=0;
    int jpv=0;
    int neutralPose=0;
    bool simpleMode=true;
    bool simpleMode2=true;
    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);
            ParamType2 paramtype = pb->GetParameterType(pid);
            char * paramName_ptr=W2A(def.int_name);
            if (ATTREQ(paramName_ptr, "thisAWDID")){
                if (paramtype==TYPE_STRING)
                    skeletonMod_ptr=W2A(pb->GetStr(pid));
            }
            if (ATTREQ(paramName_ptr, "jointPerVert")){
                if (paramtype==TYPE_INT)
                    jpv=pb->GetInt(pid);
            }
            if (ATTREQ(paramName_ptr, "neutralPose")){
                if (paramtype==TYPE_INT)
                    neutralPose=pb->GetInt(pid);
            }
            if (ATTREQ(paramName_ptr, "simpleMode")){
                if (paramtype==TYPE_BOOL)
                    simpleMode=(0 != pb->GetInt(pid));
            }
            if (ATTREQ(paramName_ptr, "simpleMode2")){
                if (paramtype==TYPE_BOOL)
                    simpleMode2=(0 != pb->GetInt(pid));
            }
            free(paramName_ptr);
        }
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    Modifier *allreadyparsedMod=(Modifier*)this->sourceModsIDsCache->Get(skeletonMod_ptr);
    if(allreadyparsedMod==NULL){
        AWDSkeleton *awdSkel = skeletonCache->Add(node, skeletonMod_ptr, neutralPose*GetTicksPerFrame());
        awdSkel->set_joints_per_vert(jpv);
        awdSkel->set_simpleMode(simpleMode);
        awdSkel->set_shareAutoAnimator(simpleMode2);
        awd->add_skeleton(awdSkel);
        char * skelName_ptr=W2A(node_mod->GetName());
        awdSkel->set_name(skelName_ptr, strlen(skelName_ptr));
        free(skelName_ptr);
        sourceObjsIDsCache->Set(skeletonMod_ptr,node);//using the custom-AWD-IDs of the custom-nodes as key in this cache, way we can access the correct source object (3dsmax-node) for each animClip later...

        AWDBlockList * thisSourceClips=(AWDBlockList *)animSourceForAnimSet->Get(skeletonMod_ptr);
        if (thisSourceClips==NULL){
            thisSourceClips = new AWDBlockList();
            animSourceForAnimSet->Set(skeletonMod_ptr, thisSourceClips);
        }
        pb = GetParamBlock2ByName((ReferenceMaker*)node_mod, "AnimClipsparams");//second params contain the AnimationClips !
        if(pb!=NULL){
            ReadAWDAnimationClips(pb, skeletonMod_ptr, thisSourceClips, ANIMTYPESKELETON);
        }
        else{
            //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
        }
        this->sourceModsIDsCache->Set(skeletonMod_ptr, node_mod);
    }
    if (skeletonMod_ptr!=NULL)
        free(skeletonMod_ptr);
    // no need to read in the SkeletonClones-rollout  and the anim-set-rollout...
}

void MaxAWDExporter::ReadAWDAnimationClips(IParamBlock2 *pb, const char * settingsNodeID_ptr, AWDBlockList * clipList, AWD_Anim_Type animType)
{
    int numBlockparams=pb->NumParams();
    int p=0;
    ParamID pid_names = NULL;
    ParamID pid_ids = NULL;
    ParamID pid_starts = NULL;
    ParamID pid_ends = NULL;
    ParamID pid_skips = NULL;
    ParamID pid_stitchs = NULL;
    ParamID pid_loops = NULL;
    ParamID pid_transforms = NULL;
    int minCount=100000;
    for (p=0; p<numBlockparams; p++) {
        ParamID pid = pb->IndextoID(p);
        ParamDef def = pb->GetParamDef(pid);
        char * paramName = W2A(def.int_name);
        if (ATTREQ(paramName, "saveAnimClipNames")){
            pid_names=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveAnimSourceIDs")){
            pid_ids=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveStartFrames")){
            pid_starts=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveEndFrames")){
            pid_ends=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveSkipFrames")){
            pid_skips=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveStitchFinals")){
            pid_stitchs=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveLoops")){
            pid_loops=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveTransform")){
            pid_transforms=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        free(paramName);
    }
    if ((pid_names==NULL)||(pid_ids==NULL)||(pid_starts==NULL)||(pid_ends==NULL)||(pid_skips==NULL)||(pid_stitchs==NULL)||(pid_loops==NULL)||(pid_transforms==NULL)){
        minCount=0;
    }
    if (minCount!=0){
        int clipCnt=0;
        for (clipCnt=0;clipCnt<minCount;clipCnt++){
            char * thisName_ptr=W2A(pb->GetStr(pid_names, 0, clipCnt));
            char * awdID_ptr=W2A(pb->GetStr(pid_ids, 0, clipCnt));
            int startFrame = pb->GetInt(pid_starts, 0, clipCnt);
            int endFrame = pb->GetInt(pid_ends, 0, clipCnt);
            int skipFrames = pb->GetInt(pid_skips, 0, clipCnt);
            bool stitchFrames = (0 != pb->GetInt(pid_skips, 0, clipCnt));
            bool loop = (0 != pb->GetInt(pid_loops, 0, clipCnt));
            bool useTransform = (0 != pb->GetInt(pid_transforms, 0, clipCnt));
            //checking if name is unique
            AWDBlock * clipForThisName = (AWDBlock*)animClipsNamesCache->Get(thisName_ptr);
            if(clipForThisName==NULL){
                if (animType==ANIMTYPEVERTEX){
                    AWDVertexAnimation * newVertexAnimClip = new AWDVertexAnimation(thisName_ptr, strlen(thisName_ptr), startFrame, endFrame, skipFrames, stitchFrames, settingsNodeID_ptr, loop, useTransform);
                    clipList->append(newVertexAnimClip);
                    animClipsIDsCache->Set(awdID_ptr, newVertexAnimClip);
                    awd->add_vertex_anim_block(newVertexAnimClip);
                    animClipsNamesCache->Set(thisName_ptr, newVertexAnimClip);
                }
                else if (animType==ANIMTYPESKELETON){
                    AWDSkeletonAnimation * newSkeletonAnimClip = new AWDSkeletonAnimation(thisName_ptr, strlen(thisName_ptr), startFrame, endFrame, skipFrames, stitchFrames, settingsNodeID_ptr, loop, useTransform);
                    clipList->append(newSkeletonAnimClip);
                    animClipsIDsCache->Set(awdID_ptr, newSkeletonAnimClip);
                    awd->add_skeleton_anim(newSkeletonAnimClip);
                    animClipsNamesCache->Set(thisName_ptr, newSkeletonAnimClip);
                }
                else if (animType==ANIMTYPEUV){
                    AWDUVAnimation * newUVAnimClip = new AWDUVAnimation(thisName_ptr, strlen(thisName_ptr), startFrame, endFrame, skipFrames, stitchFrames, settingsNodeID_ptr, loop, useTransform);
                    clipList->append(newUVAnimClip);
                    animClipsIDsCache->Set(awdID_ptr, newUVAnimClip);
                    awd->add_uv_anim(newUVAnimClip);
                    animClipsNamesCache->Set(thisName_ptr, newUVAnimClip);
                }
            }
            else{
                AWDMessageBlock * newWarning = new AWDMessageBlock(thisName_ptr, "- A Animationclip with this name allready exists.A animationclip must have a unique name!");
                awd->get_message_blocks()->append(newWarning);
            }
            free(thisName_ptr);
            free(awdID_ptr);
        }
    }
    pid_names = NULL;
    pid_ids = NULL;
    pid_starts = NULL;
    pid_ends = NULL;
    pid_skips = NULL;
    pid_stitchs = NULL;
    pid_loops = NULL;
    pid_transforms = NULL;
}

void MaxAWDExporter::ReadAWDVertexMod(Modifier *node_mod, INode * node)
{
    //output_debug_string("-> Found a active AWDVertexModifier");
    int num_params = node_mod->NumParamBlocks();
    char * vertexAnimMod_ptr = NULL;
    int p=0;
    int numBlockparams=0;
    bool simpleMode=true;
    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);
            ParamType2 paramtype = pb->GetParameterType(pid);
            char * paramName_ptr=W2A(def.int_name);
            if (ATTREQ(paramName_ptr, "thisAWDID")){
                if (paramtype==TYPE_STRING)
                    vertexAnimMod_ptr=W2A(pb->GetStr(pid));
            }
            if (ATTREQ(paramName_ptr, "neutralPose")){
                if (paramtype==TYPE_INT){
                    int neutralPose=pb->GetInt(pid);
                    vetexAnimNeutralPosecache->Set(node, neutralPose);
                }
            }
            free(paramName_ptr);
        }
    }
    else{
        //output_debug_string("!!!!! ERROR COULD NOT READ THE PARAMBLOCK FOR THE AWDVERTEXMODIFIER-MAIN_PARAMS!!!!!");
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    if(vertexAnimMod_ptr!=NULL){
        Modifier *allreadyparsedMod=(Modifier*)sourceModsIDsCache->Get(vertexAnimMod_ptr);
        if(allreadyparsedMod==NULL){
            sourceObjsIDsCache->Set(vertexAnimMod_ptr, node);//using the custom-AWD-IDs of the custom-nodes as key in this cache, way we can access the correct source object (3dsmax-node) for each animClip later...
            AWDBlockList * thisSourceClips=(AWDBlockList *)animSourceForAnimSet->Get(vertexAnimMod_ptr);
            //output_debug_string(vertexAnimMod_ptr);
            if (thisSourceClips==NULL){
                thisSourceClips = new AWDBlockList();
                animSourceForAnimSet->Set(vertexAnimMod_ptr, thisSourceClips);
            }
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 1);//second params contain the AnimationClips !
            if(pb!=NULL){
                ReadAWDAnimationClips(pb, vertexAnimMod_ptr, thisSourceClips, ANIMTYPEVERTEX);
            }
            else{
                //output_debug_string("!!!!! ERROR COULD NOT READ THE PARAMBLOCK FOR THE AWDVERTEXMODIFIER-ANIMATIONCLIPS!!!!!");
                //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
            }
            sourceModsIDsCache->Set(vertexAnimMod_ptr, node_mod);
        }
        free(vertexAnimMod_ptr);
    }
    else{}
        //output_debug_string("!!!!! ERROR COULD NOT READ THE ID FOR THE AWDVERTEXMODIFIER!!!!!");
}

void MaxAWDExporter::ReadAWDAnimSourceCloneMod(Modifier *node_mod, INode * node, AWD_Anim_Type animType){
    int num_params = node_mod->NumParamBlocks();
    char * settingsNodeID_ptr=NULL;
    char * targetID_ptr=NULL;
    int p=0;
    int numBlockparams=0;
    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);
            ParamType2 paramtype = pb->GetParameterType(pid);
            if (paramtype==TYPE_STRING)    {
                char * paramName=W2A(def.int_name);
                if (ATTREQ(paramName, "thisAWDID"))
                    settingsNodeID_ptr=W2A(pb->GetStr(pid));
                if (ATTREQ(paramName, "target_ID"))
                    targetID_ptr=W2A(pb->GetStr(pid));
                free(paramName);
            }
        }
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    sourceObjsIDsCache->Set(settingsNodeID_ptr,node);//using the custom-AWD-IDs of the custom-nodes as key in this cache, way we can access the correct source object (3dsmax-node) for each animClip later...

    AWDBlockList * thisSourceClips=(AWDBlockList *)animSourceForAnimSet->Get(targetID_ptr);
    if (thisSourceClips==NULL){
        thisSourceClips = new AWDBlockList();
        animSourceForAnimSet->Set(targetID_ptr, thisSourceClips);
    }
    pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 1);//second params contain the AnimationClips !
    if(pb!=NULL){
        ReadAWDAnimationClips(pb, settingsNodeID_ptr, thisSourceClips, animType);
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    if(settingsNodeID_ptr!=NULL)
        free(settingsNodeID_ptr);
    if(targetID_ptr!=NULL)
        free(targetID_ptr);
}

void MaxAWDExporter::ReadAWDAnimSet(Modifier *node_mod){
    ParamID pid_anim_ids = NULL;
    ParamID pid_animsource_ids = NULL;
    int minCount=100000;
    AWDAnimationSet * animSet = (AWDAnimationSet *)animSetsCache->Get(node_mod);
    if (animSet==NULL){
        // create a AnimationSet Block
        // if it is not in "simple mode", we need to get all the stored animclips-IDs and the corresponding sourceModifier-IDs
        int num_params = node_mod->NumParamBlocks();
        char * settingsNodeID_ptr=NULL;
        char * sourceSkeletonID_ptr=NULL;
        char * sourceVertexID_ptr=NULL;
        bool simpleMode=true;
        int animType=0;
        int p=0;
        IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
        if(pb!=NULL){
            int numBlockparams=pb->NumParams();
            for (p=0; p<numBlockparams; p++) {
                ParamID pid = pb->IndextoID(p);
                ParamDef def = pb->GetParamDef(pid);
                ParamType2 paramtype = pb->GetParameterType(pid);
                char * paramName=W2A(def.int_name);
                if (ATTREQ(paramName, "saveAnimIDs")){
                    pid_anim_ids=pid;
                    if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
                }
                if (ATTREQ(paramName, "saveAnimSourceIDs")){
                    pid_animsource_ids=pid;
                    if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
                }
                if (paramtype==TYPE_STRING) {
                    if (ATTREQ(paramName, "thisAWDID"))
                        settingsNodeID_ptr = W2A(pb->GetStr(pid));
                    if (ATTREQ(paramName, "sourceVertexID"))
                        sourceVertexID_ptr = W2A(pb->GetStr(pid));
                    if (ATTREQ(paramName, "sourceSkeletonID"))
                        sourceSkeletonID_ptr= W2A(pb->GetStr(pid));
                }
                if (ATTREQ(paramName, "animType")){
                    if (paramtype==TYPE_INT)
                        animType = pb->GetInt(pid);
                }
                if (ATTREQ(paramName, "simpleMode")){
                    if (paramtype==TYPE_BOOL)
                        simpleMode= (0 != pb->GetInt(pid));
                }
                free(paramName);
            }

            if ((animType==1)||(animType==2)||(animType==3)){
                char * animSetname=W2A(node_mod->GetName());
                if (animType==1)
                    animSet = new AWDAnimationSet(animSetname, strlen(animSetname),ANIMTYPESKELETON, sourceSkeletonID_ptr, strlen(sourceSkeletonID_ptr), NULL);
                else if (animType==2)
                    animSet = new AWDAnimationSet(animSetname, strlen(animSetname),ANIMTYPEVERTEX, sourceVertexID_ptr, strlen(sourceVertexID_ptr), NULL);
                else if (animType==3)
                    animSet = new AWDAnimationSet(animSetname, strlen(animSetname),ANIMTYPEUV, NULL, 0, NULL);
                if (!simpleMode){
                    animSet->set_simple_mode(true);
                    if ((pid_anim_ids==NULL)||(pid_animsource_ids==NULL))
                        minCount=0;
                    }
                    if (minCount!=0){
                        int clipCnt=0;
                        for (clipCnt=0;clipCnt<minCount;clipCnt++){
                            char * animID_ptr=W2A(pb->GetStr(pid_anim_ids, 0, clipCnt));
                            char * animSourceID_ptr=W2A(pb->GetStr(pid_animsource_ids, 0, clipCnt));
                            //this could be any type of AWDAnimClip. its only used to store the both ids, for later access
                            AWDVertexAnimation * newVertexAnimClip = new AWDVertexAnimation(animID_ptr, strlen(animID_ptr), 0, 0, 0, false, animSourceID_ptr, false, false);
                            animSet->get_preAnimationClipNodes()->append(newVertexAnimClip);
                            free(animID_ptr);
                            free(animSourceID_ptr);
                        }
                    }
                animSetsIDsCache->Set(settingsNodeID_ptr, animSet);
                animSetsCache->Set(node_mod, animSet);
                awd->add_amin_set_block(animSet);
                free(animSetname);
            }
        }
        else{
                        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
        }
        if (settingsNodeID_ptr!=NULL)
            free(settingsNodeID_ptr);
        if (sourceVertexID_ptr!=NULL)
            free(sourceVertexID_ptr);
        if (sourceSkeletonID_ptr!=NULL)
            free(sourceSkeletonID_ptr);
    }
}

AWDAnimator * MaxAWDExporter::AutoCreateAnimatorForSkeleton(INode * node){
    AWDAnimator * animatorBlock=NULL;
    Object *obj;
    obj = node->GetObjectRef();
    //getBaseObjectAndID(obj, sid);
    int skinIdx=0;
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
        // no skin = no skeleton animation
        IDerivedObject* thisBaseObj_der = (IDerivedObject*)(node->GetObjectRef());
        Object * thisOBJ=(Object *)thisBaseObj_der->GetObjRef();
        if(thisOBJ!=NULL){
            if((thisOBJ->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == DERIVOB_CLASS_ID )){
                IDerivedObject* thisDerObj=( IDerivedObject* ) thisOBJ;
                skinIdx = IndexOfSkinMod(thisOBJ, &derivedObject);
                if (skinIdx >= 0) {
                    // Flatten all modifiers up to but not including
                    // the skin modifier.
                    // to do: get the correct time for the neutral-pose
                    os = derivedObject->Eval(0, skinIdx + 1);
                }
                else{
                    return NULL;
                }
            }
            else{
                return NULL;
            }
        }
    }
    obj = os.obj;
    ISkin *skin = NULL;
    if (derivedObject != NULL && skinIdx >= 0) {
        Modifier *mod = derivedObject->GetModifier(skinIdx);
        skin = (ISkin *)mod->GetInterface(I_SKIN);
    }
    else {
        // no skin = no skeleton animation
        return NULL;
    }
    if (opts->ExportSkin() && skin && skin->GetNumBones()) {
        SkeletonCacheItem *skel = skeletonCache->GetFromBone(skin->GetBone(0));
        if (skel!=NULL){
            if (skel->awdSkel->get_simpleMode()){
                bool createAnim=false;
                AWDAnimationSet *animSet=NULL;
                if (skel->awdSkel->get_shareAutoAnimator())
                    animatorBlock = (AWDAnimator *)animatorCache->Get(skel->awdSkel);
                else{
                    animSet = (AWDAnimationSet *)animSetsIDsCache->Get(skel->awdSkel->get_awdID());
                    createAnim=true;
                }

                if ((animatorBlock==NULL)||(createAnim)){
                    if(animSet==NULL){
                        char *nodename = W2A(node->GetName());
                        char *animSetName_ptr = (char*)malloc(strlen(nodename)+9);
                        strcpy(animSetName_ptr, nodename);
                        strcat(animSetName_ptr, "_animSet");
                        animSetName_ptr[strlen(nodename)+8]=0;
                        animSet=new AWDAnimationSet(animSetName_ptr, strlen(animSetName_ptr),ANIMTYPESKELETON, skel->awdID, strlen(skel->awdID), NULL);
                        awd->add_amin_set_block(animSet);
                        if(createAnim)
                            animSetsIDsCache->Set(skel->awdSkel->get_awdID(), animSet);
                        free(animSetName_ptr);
                        free(nodename);
                    }
                    char *nodename = W2A(node->GetName());
                    char *animatorName_ptr = (char*)malloc(strlen(nodename)+10);
                    strcpy(animatorName_ptr, nodename);
                    strcat(animatorName_ptr, "_animator");
                    animatorName_ptr[strlen(nodename)+9]=0;
                    animatorBlock = new AWDAnimator(animatorName_ptr, strlen(animatorName_ptr),animSet, ANIMTYPESKELETON);
                    awd->add_animator(animatorBlock);
                    if (skel->awdSkel->get_shareAutoAnimator())
                        animatorCache->Set(skel->awdSkel, animatorBlock);
                    free(animatorName_ptr);
                    free(nodename);
                    return animatorBlock;
                }
                else{
                    // allready has animator for this skeleton
                    return animatorBlock;
                }
            }
            else {
                // not in simple mode -  no skeleton animation
                return NULL;
            }
        }
        else {
            // no skeleton found in cache!!! -  no skeleton animation
            return NULL;
        }
    }
    else {
        // no skin = no skeleton animation
        return NULL;
    }
    return animatorBlock;
}

AWDAnimator * MaxAWDExporter::AutoCreateAnimatorForVertexAnim(INode * node){
    AWDAnimator * animatorBlock=NULL;
    BaseObject* node_bo = node->GetObjectRef();
    if((node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == DERIVOB_CLASS_ID ))
    {
        IDerivedObject* node_der = (IDerivedObject*)(node_bo);
        if (node_der!=NULL){
            int nMods = node_der->NumModifiers();
            for (int m = 0; m<nMods; m++){
                Modifier* node_mod = node_der->GetModifier(m);
                if (node_mod->IsEnabled()){
                    MSTR className;
                    node_mod->GetClassName(className);
                    char * className_ptr=W2A(className);
                    if (ATTREQ(className_ptr,"AWDVertexAnimSource")){
                        free(className_ptr);
                        animatorBlock=ReadAWDVertexModForMesh(node_mod, node);
                        if (animatorBlock!=NULL)
                           return animatorBlock;
                    }
                    else{
                        free(className_ptr);
                    }
                }
            }
        }
        if (animatorBlock==NULL){
            // no skin = no skeleton animation
            IDerivedObject* thisBaseObj_der = (IDerivedObject*)(node->GetObjectRef());
            Object * thisOBJ=(Object *)thisBaseObj_der->GetObjRef();
            if(thisOBJ!=NULL){
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
                                if (ATTREQ(className_ptr,"AWDVertexAnimSource")){
                                    free(className_ptr);
                                    animatorBlock=ReadAWDVertexModForMesh(node_mod, node);
                                    if (animatorBlock!=NULL)
                                       return animatorBlock;
                                }
                                else
                                    free(className_ptr);
                            }
                        }
                    }
                }
            }
        }
    }
    return animatorBlock;
}

AWDAnimator * MaxAWDExporter::AutoCreateAnimatorForUV(INode * node, AWDBlockList * matList){
    AWDAnimator * animatorBlock=NULL;
    AWDBlockList * newClipList = new AWDBlockList();
    AWDMaterial * awdMat=NULL;
    AWDBlockIterator *itMat;
    itMat = new AWDBlockIterator(matList);
    while ((awdMat = (AWDMaterial*)itMat->next()) != NULL) {
        AWDBitmapTexture * awdtex= awdMat->get_texture();
        if(awdtex!=NULL){
            if(awdtex->get_uvAnimSourceId_len()>0){
                AWDBlockList * newClips=(AWDBlockList *)animSourceForAnimSet->Get(awdtex->get_uvAnimSourceId());
                if (newClips!=NULL){
                    if(newClips->get_num_blocks()>0){
                        AWDBlock * awdClip=NULL;
                        AWDBlockIterator *itClips;
                        itClips = new AWDBlockIterator(newClips);
                        while ((awdClip = (AWDBlock*)itClips->next()) != NULL) {
                            newClipList->append(awdClip);
                        }
                        delete itClips;
                    }
                }
            }
        }
        awdtex=NULL;
        awdtex= awdMat->get_specTexture();
        if(awdtex!=NULL){
            if(awdtex->get_uvAnimSourceId_len()>0){
                AWDBlockList * newClips=(AWDBlockList *)animSourceForAnimSet->Get(awdtex->get_uvAnimSourceId());
                if (newClips!=NULL){
                    if(newClips->get_num_blocks()>0){
                        AWDBlock * awdClip=NULL;
                        AWDBlockIterator *itClips;
                        itClips = new AWDBlockIterator(newClips);
                        while ((awdClip = (AWDBlock*)itClips->next()) != NULL) {
                            newClipList->append(awdClip);
                        }
                        delete itClips;
                    }
                }
            }
        }
        awdtex=NULL;
        awdtex= awdMat->get_ambientTexture();
        if(awdtex!=NULL){
            if(awdtex->get_uvAnimSourceId_len()>0){
                AWDBlockList * newClips=(AWDBlockList *)animSourceForAnimSet->Get(awdtex->get_uvAnimSourceId());
                if (newClips!=NULL){
                    if(newClips->get_num_blocks()>0){
                        AWDBlock * awdClip=NULL;
                        AWDBlockIterator *itClips;
                        itClips = new AWDBlockIterator(newClips);
                        while ((awdClip = (AWDBlock*)itClips->next()) != NULL) {
                            newClipList->append(awdClip);
                        }
                        delete itClips;
                    }
                }
            }
        }
        awdtex=NULL;
        awdtex= awdMat->get_normalTexture();
        if(awdtex!=NULL){
            if(awdtex->get_uvAnimSourceId_len()>0){
                AWDBlockList * newClips=(AWDBlockList *)animSourceForAnimSet->Get(awdtex->get_uvAnimSourceId());
                if (newClips!=NULL){
                    if(newClips->get_num_blocks()>0){
                        AWDBlock * awdClip=NULL;
                        AWDBlockIterator *itClips;
                        itClips = new AWDBlockIterator(newClips);
                        while ((awdClip = (AWDBlock*)itClips->next()) != NULL) {
                            newClipList->append(awdClip);
                        }
                        delete itClips;
                    }
                }
            }
        }
    }
    delete itMat;
    if(newClipList->get_num_blocks()>0){
        char *nodename = W2A(node->GetName());
        char *animSetName_ptr = (char*)malloc(strlen(nodename)+9);
        strcpy(animSetName_ptr, nodename);
        strcat(animSetName_ptr, "_animSet");
        animSetName_ptr[strlen(nodename)+8]=0;
        AWDAnimationSet *animSet=new AWDAnimationSet(animSetName_ptr, strlen(animSetName_ptr),ANIMTYPEUV, NULL, 0, newClipList);
        awd->add_amin_set_block(animSet);
        free(animSetName_ptr);
        char *animatorName_ptr = (char*)malloc(strlen(nodename)+10);
        strcpy(animatorName_ptr, nodename);
        strcat(animatorName_ptr, "_animator");
        animatorName_ptr[strlen(nodename)+9]=0;
        animatorBlock = new AWDAnimator(animatorName_ptr, strlen(animatorName_ptr), animSet, ANIMTYPEUV);
        awd->add_animator(animatorBlock);
        //animatorCache->Set(skel->awdSkel, animatorBlock);
        free(animatorName_ptr);
        free(nodename);
        return animatorBlock;
    }

    return animatorBlock;
}

AWDAnimator * MaxAWDExporter::ReadAWDVertexModForMesh(Modifier *node_mod, INode * node){
    AWDAnimator * animatorBlock=NULL;
    int num_params = node_mod->NumParamBlocks();
    char * settingsNodeID=NULL;
    int p=0;
    bool simpleMode=false;
    bool simpleMode2=false;
    int numBlockparams=0;
    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);
            ParamType2 paramtype = pb->GetParameterType(pid);
            char * paramName = W2A(def.int_name);
            if (ATTREQ(paramName, "thisAWDID")){
                if (paramtype==TYPE_STRING)
                    settingsNodeID=W2A(pb->GetStr(pid));
            }
            if (ATTREQ(paramName, "simpleMode")){
                if (paramtype==TYPE_BOOL)
                    simpleMode=(0 != pb->GetInt(pid));
            }
            if (ATTREQ(paramName, "simpleMode2")){
                if (paramtype==TYPE_BOOL)
                    simpleMode2=(0 != pb->GetInt(pid));
            }
            free(paramName);
        }
        if (simpleMode){
            bool createAnim=false;
            AWDAnimationSet *animSet=NULL;
            if (simpleMode2)
                animatorBlock = (AWDAnimator *)animatorCache->Get(node_mod);
            else{
                animSet = (AWDAnimationSet *)animSetsIDsCache->Get(settingsNodeID);
                createAnim=true;
            }
            if ((animatorBlock==NULL)||(createAnim)){
                if(animSet==NULL){
                    char *nodename = W2A(node->GetName());
                    char *animSetName_ptr = (char*)malloc(strlen(nodename)+9);
                    strcpy(animSetName_ptr, nodename);
                    strcat(animSetName_ptr, "_animSet");
                    animSetName_ptr[strlen(nodename)+8]=0;
                    animSet=new AWDAnimationSet(animSetName_ptr, strlen(animSetName_ptr),ANIMTYPEVERTEX, settingsNodeID, strlen(settingsNodeID), NULL);
                    awd->add_amin_set_block(animSet);
                    animSetsIDsCache->Set(settingsNodeID, animSet);
                    free(animSetName_ptr);
                    free(nodename);
                }
                char *nodename = W2A(node->GetName());
                char *animatorName_ptr = (char*)malloc(strlen(nodename)+10);
                strcpy(animatorName_ptr, nodename);
                strcat(animatorName_ptr, "_animator");
                animatorName_ptr[strlen(nodename)+9]=0;
                animatorBlock = new AWDAnimator(animatorName_ptr, strlen(animatorName_ptr),animSet, ANIMTYPEVERTEX);
                awd->add_animator(animatorBlock);
                animatorCache->Set(node_mod, animatorBlock);
                free(animatorName_ptr);
                free(nodename);
            }
        }
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    if(settingsNodeID!=NULL)
        free(settingsNodeID);
    return animatorBlock;
}

AWDAnimator * MaxAWDExporter::GetAWDAnimatorForObject(INode *node){
    AWDAnimator * animatorBlock=NULL;
    BaseObject* node_bo = node->GetObjectRef();
    if((node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (node_bo->SuperClassID() == DERIVOB_CLASS_ID ))
    {
        Modifier* anim_Mod=NULL;
        AWDAnimationSet *animSet=NULL;
        bool createUnique=false;
        char * settingsNodeID_ptr=NULL;
        char * animSetID_ptr=NULL;
        char * animatorName=NULL;
        IDerivedObject* node_der = (IDerivedObject*)(node_bo);
        if (node_der!=NULL){
            int nMods = node_der->NumModifiers();
            for (int m = 0; m<nMods; m++){
                Modifier* node_mod = node_der->GetModifier(m);
                if (node_mod->IsEnabled()){
                    //DebugPrint("node_mod.IsEnabled() = "+node_mod->IsEnabled());
                    //MSTR name=node_mod->GetName();
                    MSTR className;
                    node_mod->GetClassName(className);
                    char * className_ptr=W2A(className);
                    if (ATTREQ(className_ptr,"AWDAnimator") ){
                        anim_Mod=node_mod;
                        animatorName=W2A(node_mod->GetName());
                        int num_params = node_mod->NumParamBlocks();
                        int p=0;
                        IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
                        if(pb!=NULL){
                            int numBlockparams=pb->NumParams();
                            for (p=0; p<numBlockparams; p++) {
                                ParamID pid = pb->IndextoID(p);
                                ParamDef def = pb->GetParamDef(pid);
                                ParamType2 paramtype = pb->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (ATTREQ(paramName_ptr, "thisAWDID") ){
                                    if (paramtype==TYPE_STRING)
                                        settingsNodeID_ptr = W2A(pb->GetStr(pid));
                                }
                                if (ATTREQ(paramName_ptr, "AnimationSetID") ){
                                    if (paramtype==TYPE_STRING)
                                        animSetID_ptr = W2A(pb->GetStr(pid));
                                }
                                if (ATTREQ(paramName_ptr, "createUnique") ){
                                    if (paramtype==TYPE_BOOL)
                                        createUnique=(0 != pb->GetInt(pid));
                                }
                                free(paramName_ptr);
                            }
                        }
                        if(animSetID_ptr!=NULL){
                            animSet=(AWDAnimationSet *)animSetsIDsCache->Get(animSetID_ptr);
                        }
                    }
                    free(className_ptr);
                }
            }
            if(animSet==NULL){
                IDerivedObject* thisBaseObj_der = (IDerivedObject*)(node->GetObjectRef());
                Object * thisOBJ=(Object *)thisBaseObj_der->GetObjRef();
                if((thisOBJ->SuperClassID() == GEN_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == WSM_DERIVOB_CLASS_ID) || (thisOBJ->SuperClassID() == DERIVOB_CLASS_ID )){
                    IDerivedObject* thisDerObj=( IDerivedObject* ) thisOBJ;
                    if (thisDerObj!=NULL){
                        int nMods = thisDerObj->NumModifiers();
                        for (int m = 0; m<nMods; m++){
                            Modifier* node_mod = thisDerObj->GetModifier(m);
                            if (node_mod->IsEnabled()){
                                //DebugPrint("node_mod.IsEnabled() = "+node_mod->IsEnabled());
                                //MSTR name=node_mod->GetName();
                                MSTR className;
                                node_mod->GetClassName(className);
                                char * className_ptr=W2A(className);
                                if (ATTREQ(className_ptr,"AWDAnimator") ){
                                    anim_Mod=node_mod;
                                    animatorName=W2A(node_mod->GetName());
                                    int num_params = node_mod->NumParamBlocks();
                                    int p=0;
                                    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
                                    if(pb!=NULL){
                                        int numBlockparams=pb->NumParams();
                                        for (p=0; p<numBlockparams; p++) {
                                            ParamID pid = pb->IndextoID(p);
                                            ParamDef def = pb->GetParamDef(pid);
                                            ParamType2 paramtype = pb->GetParameterType(pid);
                                            char * paramName_ptr=W2A(def.int_name);
                                            if (ATTREQ(paramName_ptr, "thisAWDID") ){
                                                if (paramtype==TYPE_STRING)
                                                    settingsNodeID_ptr = W2A(pb->GetStr(pid));
                                            }
                                            if (ATTREQ(paramName_ptr, "AnimationSetID") ){
                                                if (paramtype==TYPE_STRING)
                                                    animSetID_ptr = W2A(pb->GetStr(pid));
                                            }
                                            if (ATTREQ(paramName_ptr, "createUnique") ){
                                                if (paramtype==TYPE_BOOL)
                                                    createUnique=(0 != pb->GetInt(pid));
                                            }
                                            free(paramName_ptr);
                                        }
                                    }
                                    if(animSetID_ptr!=NULL){
                                        animSet=(AWDAnimationSet *)animSetsIDsCache->Get(animSetID_ptr);
                                    }
                                }
                                free(className_ptr);
                            }
                        }
                    }
                }
            }
            if(animatorName!=NULL){
                if(animSet==NULL){
                    AWDMessageBlock * newWarning = new AWDMessageBlock(animatorName, "Could not find the animationset-id for this animator");
                    awd->get_message_blocks()->append(newWarning);
                }
                else{
                    animatorBlock = (AWDAnimator *)animatorCache->Get(anim_Mod);
                    if ((createUnique)||(animatorBlock==NULL)){
                        animatorBlock = new AWDAnimator(animatorName, strlen(animatorName),animSet, animSet->get_anim_type());
                        animatorCache->Set(anim_Mod, animatorBlock);
                        awd->add_animator(animatorBlock);
                    }
                }
                free(animatorName);
            }
        }
        if(settingsNodeID_ptr!=NULL)
            free(settingsNodeID_ptr);
        if(animSetID_ptr!=NULL)
            free(animSetID_ptr);
    }
    return animatorBlock;
}

int MaxAWDExporter::ExportSkin(INode *node, ISkin *skin, awd_float64 **extWeights, awd_uint32 **extJoints)
{
    if (opts->ExportSkin() && skin && skin->GetNumBones()) {
        int iVtx=0;
        awd_float64 *weights;
        awd_uint32 *indices;

        // Get skeleton information from cache and geometry information
        // through an ISkinContextData interface.
        SkeletonCacheItem *skel = skeletonCache->GetFromBone(skin->GetBone(0));
        ISkinContextData *context = skin->GetContextInterface(node);

        // If the skeleton used for this skin could not be found,
        // break now or the code below will crash
        if (skel == NULL){
            return 0;
        }
        int jointsPerVertex = 0;
        if (skel->awdSkel->get_joints_per_vert()!=0){
            jointsPerVertex = skel->awdSkel->get_joints_per_vert();
        }

        int numVerts = context->GetNumPoints();
        weights = (awd_float64*)malloc(jointsPerVertex * numVerts * sizeof(awd_float64));
        indices = (awd_uint32*)malloc(jointsPerVertex * numVerts * sizeof(awd_uint32));

        for (iVtx=0; iVtx<numVerts; iVtx++) {
            int iBone=0;
            int iWeight=0;
            int numBones=0;
            double weightSum = 0;
            numBones = context->GetNumAssignedBones(iVtx);
            if (numBones>0){
                double *tmpWeights = (double*)malloc(numBones*sizeof(double));
                int *tmpIndices = (int*)malloc(numBones*sizeof(int));
                // Retrieve weight/index for all joints in skin.
                for (iBone=0; iBone<numBones; iBone++) {
                    double thisWeight=double(0.0);
                    int thisIdx=int(0);
                    // do we need the try/catch ?
                    // Was included while searching the bug that cashed crash when skin had not "removed zero weights"
                    // need to test if this is really needed
                    try{
                        int boneId = context->GetAssignedBone(iVtx, iBone);
                        INode *bone = skin->GetBone(boneId);
                        thisWeight = double(context->GetBoneWeight(iVtx, iBone));
                        thisIdx = int(skel->IndexOfBone(bone));
                    }
                    catch(...){
                        thisWeight=double(0.0);
                        thisIdx=int(0);
                    }
                    if (thisWeight==NULL)
                        thisWeight=double(0.0);
                    if (thisIdx==NULL)
                        thisIdx=int(0);

                    tmpWeights[iBone] = double(thisWeight);
                    tmpIndices[iBone] = int(thisIdx);
                }
                // Retrieve most significant joint weights from temporary buffers
                // or after having run out of assigned bones for a vertex, set
                // weight to zero.
                for (iWeight=0; iWeight < jointsPerVertex; iWeight++) {
                    int strIdx = iVtx*jointsPerVertex + iWeight;

                    if (iWeight < numBones) {
                        int maxIBone = -1;
                        double max = 0.0;

                        // Loop through temporary buffer to find most significant
                        // joint (highest weight) and store it.
                        for (iBone=0; iBone<numBones; iBone++) {
                            if (tmpWeights[iBone] > max) {
                                max = tmpWeights[iBone];
                                maxIBone = iBone;
                            }
                        }

                        // Retrieve most significant weight/index pair
                        weights[strIdx] = tmpWeights[maxIBone];
                        indices[strIdx] = tmpIndices[maxIBone];

                        weightSum += weights[strIdx];

                        // Set to zero to mark as already used. (only if maxIbone is a valid idx)
                        if(maxIBone>=0)
                            tmpWeights[maxIBone] = double(0.0);
                    }
                    else {
                        weights[strIdx] = 0.0;
                        indices[strIdx] = 0;
                    }
                }

                // if the sum of the weights was 0.0, than  skin the vert to the root-joint:
                if (weightSum<=0){
                    weights[iVtx*jointsPerVertex + 0] = 1.0;
                    indices[iVtx*jointsPerVertex + 0] = 0;
                }
                else{
                    // Normalize weights (sum must be 1.0)
                    double scale = 1/weightSum;
                    for (iBone=0; iBone<jointsPerVertex; iBone++) {
                        weights[iVtx*jointsPerVertex + iBone] *= scale;
                    }
                }
                free(tmpWeights);
                free(tmpIndices);
            }
            else{
                weights[iVtx*jointsPerVertex + 0] = 1.0;
                indices[iVtx*jointsPerVertex + 0] = 0;
                for (iWeight=1; iWeight < jointsPerVertex; iWeight++) {
                    weights[iVtx*jointsPerVertex + iWeight] = 0.0;
                    indices[iVtx*jointsPerVertex + iWeight] = 0;
                }
            }
        }

        *extWeights = weights;
        *extJoints = indices;

        return jointsPerVertex;
    }

    return 0;
}

AWDBlock * MaxAWDExporter::ExportAWDVertexClip( AWDTriGeom  * targetGeom, AWDVertexAnimation * vertClip, BlockSettings* blockSettings)
{
    // each VertexClip must know its target-Geo
    if (vertClip==NULL)
        return NULL;
    if (vertClip->get_is_processed())
        return vertClip;
    vertClip->set_is_processed(true);
    char * thisSourceID=vertClip->get_sourceID();
    if(thisSourceID==NULL)
        return NULL;
    //output_debug_string(thisName);
    INode * sourceObj = (INode *) sourceObjsIDsCache->Get(thisSourceID);
    if (sourceObj==NULL)
        return NULL;
    vertClip->set_targetGeo(targetGeom);
    int ticksPerFrame=0;
    int frameDur=0;
    int thisframeDur=0;
    ticksPerFrame = GetTicksPerFrame();
    frameDur = floor(TicksToSec(ticksPerFrame) * 1000.0 + 0.5); // ms
    int start = vertClip->get_start_frame();
    int end = vertClip->get_end_frame();
    int skip = vertClip->get_skip_frame();
    // Loop through frames for this sequence and create poses
    int f=0;
    int skipf=0;
    bool transform=vertClip->get_use_transforms();
    int neutral_pose=start*ticksPerFrame;//neutral pose is startframe of the mesh
    AWD_field_type awFieldType=AWD_FIELD_FLOAT32;
    if (blockSettings->get_wide_geom())
        awFieldType=AWD_FIELD_FLOAT64;
    Matrix3 offsMtx = sourceObj->GetObjectTM(neutral_pose) * Inverse(sourceObj->GetNodeTM(neutral_pose));

    int exportedFrames=0;

    if (skip>0)
        skip+=1;

    for (f=start; f<=end; f++) {
        skipf=f+skip;
        thisframeDur=frameDur;
        while (f<(end-1)&&(f<=skipf)){
            f++;
            thisframeDur+=frameDur;
        }
        TimeValue t = f * ticksPerFrame;
        AWDVertexGeom *pose;
        AWDVertexGeom *lastPose=vertClip->get_last_frame_geo();
        pose = new AWDVertexGeom();
        Object * obj = sourceObj->GetObjectRef();
        SClass_ID sid;
        getBaseObjectAndID(obj, sid);
        ObjectState os;
        // Flatten entire modifier stack
        os = sourceObj->EvalWorldState(t);
        obj = os.obj;
        TriObject *triObject = (TriObject*)obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));
        Mesh mesh = triObject->mesh;
        AWDSubGeom *sub;
        if(transform)
            offsMtx = (sourceObj->GetObjectTM(t) * Inverse(sourceObj->GetNodeTM(neutral_pose)));
        // Write all sub-meshes
        // for each verticle in a submesh,
        // get the original vertex-index, than get the current position...)
        sub = targetGeom->get_first_sub();

        int subCnt=0;
        bool different=true;
        while (sub) {
            AWD_str_ptr v_str;
            AWD_str_ptr originalIdxStream = sub->get_original_idx_data();
            awd_uint32 orgIdxLength = sub->get_original_idx_data_len()  *3;
            AWD_str_ptr vo_str;
            AWDSubGeom *originalsub=NULL;
            if(lastPose!=NULL){
                if(lastPose->get_num_subs()>subCnt){
                    originalsub=lastPose->get_sub_at(subCnt);
                    if(originalsub!=NULL){
                        vo_str=originalsub->get_stream_at(0)->data;
                    }
                }
            }
            v_str.v = malloc(sizeof(awd_float64) * orgIdxLength);
            different=true;
            if(originalsub!=NULL)
                different=false;
            for (int e=0; e<orgIdxLength/3; e++) {
                awd_uint32 *p = (originalIdxStream.ui32 + (e));
                awd_uint16 elem = UI16((awd_uint16)*p);
                Point3 newPoint =  offsMtx * mesh.getVert(elem);
                v_str.f64[e*3+0] = newPoint.x;
                v_str.f64[e*3+1] = newPoint.z;
                v_str.f64[e*3+2] = newPoint.y;
                if(!different){
                    if((vo_str.f64[e*3+0]!=v_str.f64[e*3+0])||(vo_str.f64[e*3+1]!=v_str.f64[e*3+1])||(vo_str.f64[e*3+2]!=v_str.f64[e*3+2])){
                        different=true;
                    }
                }
            }
            subCnt++;
            AWDSubGeom * newVertexSub=new AWDSubGeom(NULL);
            newVertexSub->add_stream(VERTICES, awFieldType, v_str, orgIdxLength);
            pose->add_sub_mesh(newVertexSub);
            sub = sub->next;
        }
        if(different){
            exportedFrames++;
            vertClip->set_next_frame_pose(pose, thisframeDur);
        }
        else{
            vertClip->append_duration_to_last_frame(thisframeDur);
        }
    }
    if(exportedFrames<=1){
        AWDMessageBlock * newWarning = new AWDMessageBlock(vertClip->get_name(), "- No keyframes read for animationclip. animationclip not exported");
        awd->get_message_blocks()->append(newWarning);
        vertClip->make_invalide();
        return NULL;
    }
    return vertClip;
}

AWDBlock * MaxAWDExporter::ExportAWDSkeletonClip(SkeletonCacheItem * skelCacheItem, AWDSkeletonAnimation * skelClip)
{
    if (skelCacheItem==NULL)
        return NULL;//ERROR NO skelCacheItem AVAILABLE
    if (skelClip==NULL)
        return NULL;//ERROR NO CLIP AVAILABLE
    if (skelClip->get_is_processed())
        return skelClip;
    skelClip->set_is_processed(true);
    char * sourceID=skelClip->get_sourceID();
    if(sourceID==NULL)
        return NULL;
    INode * sourceObj = (INode *) sourceObjsIDsCache->Get(skelClip->get_sourceID());
    if (sourceObj==NULL){
        AWDMessageBlock * newWarning = new AWDMessageBlock(skelClip->get_name(), "- Could not find the animation-source (skeleton) for this animationSet");
        awd->get_message_blocks()->append(newWarning);
        skelClip->make_invalide();
        return NULL;
    }

    int ticksPerFrame=0;
    int frameDur=0;
    int thisframeDur=0;
    ticksPerFrame = GetTicksPerFrame();
    frameDur = floor(TicksToSec(ticksPerFrame) * 1000.0 + 0.5); // ms
    int start = skelClip->get_start_frame();
    int end = skelClip->get_end_frame();
    int skip = skelClip->get_skip_frame();

    bool transform=skelClip->get_use_transforms();
    skelCacheItem->IterReset();
    int numJoints=skelCacheItem->get_num_joints();
    INode * rootBone = skelCacheItem->IterNext()->maxBone;
    if(rootBone==NULL){
        AWDMessageBlock * newWarning = new AWDMessageBlock(skelClip->get_name(), "- Could not find rootjoint for skeleton for this animationSet");
        awd->get_message_blocks()->append(newWarning);
        skelClip->make_invalide();
        return NULL; //ERROR
    }
    int neutralTime=skelCacheItem->awdSkel->get_neutralPose();
    int exportedFrames=0;
    skelCacheItem->IterReset();
    SkeletonCacheJoint *curJoint;
    AWDSkeletonPose *pose;
    AWDSkeletonPose *lastPose=NULL;
    if (sourceObj==rootBone){
        skelCacheItem->IterReset();
        if (skelClip->get_skip_frame()<0){
            // TODO:only read if
         /*   // only create poses for frames with keyframes
            skelCacheItem->IterReset();
            int cnt=0;
            int numFrames=2;
            bool exportThisFrame=false;
            IntCache * newFrameCache=new IntCache();
            IKey* mykey=NULL;
            TimeValue startT = start * ticksPerFrame;
            newFrameCache->Set(mykey, startT);
            TimeValue endT = end * ticksPerFrame;
            newFrameCache->Set(mykey, endT);
            while ((curJoint = skelCacheItem->IterNext()) != NULL) {
                INode * bone = curJoint->maxBone;
                Control *c;
                c = bone->GetTMController()->GetPositionController();
                IKeyControl *ikeys = GetKeyControlInterface(c);
                if (ikeys){
                    int numkeys = rootBone->NumKeys();
                    for (int key=0;key < numkeys; key++)
                    {
                        ikeys->GetKey(key,mykey);
                        TimeValue keytime;
                        keytime = mykey->time;
                        IKey * keyExists=newFrameCache->Get(keytime);
                        if(keyExists==NULL){
                            newFrameCache->Set(, keytime);
                            numFrames++;
                        }
                    }
                }
                cnt++;
            }
            int keyCnt=0;
            for (keyCnt=0; keyCnt<=numFrames; keyCnt++) {
                //newFrameCache->
            }*/
        }

        else{
            int f=0;
            int skipf=0;

            if (skip>0)
                skip+=1;

            for (f=start; f<=end; f++) {
                skipf=f+skip;
                thisframeDur=frameDur;
                while (f<(end-1)&&(f<skipf)){
                    f++;
                    thisframeDur+=frameDur;
                }
                TimeValue t = f * ticksPerFrame;
                char * posename=(char *)malloc(13);
                strcpy(posename, "SkeletonPose");
                posename[12]=0;
                pose = new AWDSkeletonPose(posename, strlen(posename));
                free(posename);
                skelCacheItem->IterReset();
                int cnt=0;
                bool exportThisFrame=false;
                lastPose=skelClip->get_last_frame();
                Matrix3 rootParentNeutralMtx=rootBone->GetParentTM(neutralTime);// * Inverse(rootBone->GetParentTM(neutralTime));
                rootParentNeutralMtx.NoScale(); // get rid of the scale part of the parent matrix
                rootParentNeutralMtx=Inverse(rootParentNeutralMtx);
                while ((curJoint = skelCacheItem->IterNext()) != NULL) {
                    awd_float64 *lastJointPose=NULL;
                    if (lastPose!=NULL)
                        lastJointPose = lastPose->get_transform_by_idx(cnt);
                    INode * bone = curJoint->maxBone;
                    Matrix3 parentMtx=bone->GetParentTM(t);
                    parentMtx.NoScale(); // get rid of the scale part of the parent matrix
                    Matrix3 tm = (bone->GetNodeTM(t)) * Inverse(parentMtx);// * inverseOffsetMatrix;
                    if(cnt==0){
                        if(!transform){
                            tm.IdentityMatrix();
                        }
                        else{
                            tm = (bone->GetNodeTM(t)) * rootParentNeutralMtx;
                        }
                    }
                    awd_float64 *mtx = (awd_float64*)malloc(sizeof(awd_float64)*12);
                    SerializeMatrix3(tm, mtx);
                    bool exportthis=true;
                    if(lastJointPose!=NULL){
                        exportthis=false;
                        int mtxCnt=0;
                        for(mtxCnt=0;mtxCnt<12; mtxCnt++){
                            if (lastJointPose[mtxCnt]!=mtx[mtxCnt]){
                                exportthis=true;
                            }
                        }
                    }
                    if (exportthis)
                        exportThisFrame=true;
                    pose->set_next_transform(mtx);
                    cnt++;
                }
                if(exportThisFrame){
                    skelClip->set_next_frame_pose(pose, thisframeDur);
                    exportedFrames++;
                    awd->add_skeleton_pose(pose);
                }
                if(!exportThisFrame){
                    skelClip->append_duration_to_last_frame(thisframeDur);
                    delete pose;
                }
            }
        }
    }
    else{
        int numChilds=CalcNumDescendants(sourceObj);
        if (numChilds!=numJoints){
            return NULL; //ERROR - SourceObject not cambatible to sourceSkeleton
        }
        // Loop through frames for this sequence and create poses
        int f=0;
        int skipf=0;

        int maxNumKeys=(end-start)+1;
        int thisFrameDuration=frameDur;
        int thisNumKeys=maxNumKeys;
        int totalDuration=maxNumKeys * frameDur;
        if (skip>0){
            skip+=1;
            thisNumKeys=2+((maxNumKeys-2)/skip);
            thisFrameDuration=totalDuration/thisNumKeys;
        }
        TimeValue t=start * frameDur;
        TimeValue durationEnd=end * frameDur;

        int cnt=0;
        bool exportThisFrame=false;
        while (t<=durationEnd){
            lastPose=skelClip->get_last_frame();
            AWDSkeletonPose *pose;
            char * posename=(char *)malloc(13);
            strcpy(posename, "SkeletonPose");
            posename[12]=0;
            pose = new AWDSkeletonPose(posename, strlen(posename));
            free(posename);
            // to do: check if there has been any movment (compare to last frame)
            read_transform_position_into_Pose(sourceObj, t, pose);
            skelClip->set_next_frame_pose(pose, thisFrameDuration);
            awd->add_skeleton_pose(pose);
            t+=thisFrameDuration;
        }
    }
    if(exportedFrames<=1){
        AWDMessageBlock * newWarning = new AWDMessageBlock(skelClip->get_name(), "- No keyframes read for animationclip. animationclip not exported");
        awd->get_message_blocks()->append(newWarning);
        skelClip->make_invalide();
        return NULL;
    }

    return skelClip;
}

AWDBlock * MaxAWDExporter::ExportUVClip(AWDUVAnimation * uvClip)
{
    if (uvClip==NULL)
        return NULL;//ERROR NO CLIP AVAILABLE
    if (uvClip->get_is_processed())
        return uvClip;

    uvClip->set_is_processed(true);
    char * sourceID=uvClip->get_sourceID();
    if(sourceID==NULL){
        uvClip->make_invalide();
        return NULL;
    }
    StdUVGen * sourceObj = (StdUVGen *) sourceObjsIDsCache->Get(sourceID);
    if (sourceObj==NULL){
        uvClip->make_invalide();
        return NULL;
    }

    int ticksPerFrame=0;
    int frameDur=0;
    int thisframeDur=0;
    ticksPerFrame = GetTicksPerFrame();
    frameDur = floor(TicksToSec(ticksPerFrame) * 1000.0 + 0.5); // ms
    int start = uvClip->get_start_frame();
    int end = uvClip->get_end_frame();
    int skip = 1 + uvClip->get_skip_frame();
    // Loop through frames for this sequence and create poses
    int f=0;
    int skipf=0;
    awd_float64 *ouvmtx = NULL;
    int animCnt=0;
    char * posename=(char *)malloc(7);
    strcpy(posename, "UVPose");
    posename[6]=0;
    for (f=start; f<=end; f++) {
        skipf=f+skip;
        thisframeDur=frameDur;
        while (f<(end-1)&&(f<=skipf)){
            f++;
            thisframeDur+=frameDur;
        }
        ouvmtx=uvClip->get_last_frame_tf();
        SkeletonCacheJoint *curJoint;
        AWDSkeletonPose *pose;
        TimeValue t = f * ticksPerFrame;
        awd_float64 *uvmtx = (awd_float64 *)malloc(6*sizeof(awd_float64));
        uvmtx[0]=sourceObj->GetUScl(t);//a:scalex
        uvmtx[1]=(sourceObj->GetAng(t));//b:rot1
        uvmtx[2]=(sourceObj->GetAng(t)*-1);//d:rot2
        uvmtx[3]=sourceObj->GetVScl(t);//c:scaley
        uvmtx[4]=sourceObj->GetUOffs(t); //0.0;//tx:offsetx
        uvmtx[5]=sourceObj->GetVOffs(t); //0.0;//ty:offsety
        bool exportthis=true;
        if(ouvmtx!=NULL){
            exportthis=false;
            if((ouvmtx[0]!=uvmtx[0])||(ouvmtx[1]!=uvmtx[1])||
                (ouvmtx[2]!=uvmtx[2])||(ouvmtx[3]!=uvmtx[3])||
                (ouvmtx[4]!=uvmtx[4])||(ouvmtx[5]!=uvmtx[5])){
                exportthis=true;
            }
        }
        if(exportthis){
            animCnt++;
            uvClip->set_next_frame_tf(uvmtx, thisframeDur);
        }
        else
            uvClip->append_duration_to_last_frame(thisframeDur);
    }
    free(posename);
    if(animCnt==0){
        uvClip->make_invalide();
        return NULL;
    }
    return uvClip;
}

AWDBlock * MaxAWDExporter::ExportAWDAnimSet(AWDAnimationSet * animSet, AWDBlockList * targetGeos, BlockSettings * blockSettings)
{
    if (animSet==NULL)
        return NULL;

    AWDBlockList * targetGeosList = new AWDBlockList();
    AWDMeshInst *block;
    AWDBlockIterator *itMeshes;
    itMeshes = new AWDBlockIterator(targetGeos);
    while ((block = (AWDMeshInst * )itMeshes->next()) != NULL) {
        targetGeosList->append(block->get_geom());
    }
    delete itMeshes;

    AWD_Anim_Type animType=animSet->get_anim_type();

    if (animSet->get_is_processed()){
        /*AWDTriGeom *block;
        itMeshes = new AWDBlockIterator(targetGeosList);
        while ((block = (AWDTriGeom * )itMeshes->next()) != NULL) {
            if (animType==ANIMTYPESKELETON){
            }
            else if (animType==ANIMTYPEVERTEX){
                if (block->get_originalPointCnt()!=0){
                    if (animSet->get_originalPointCnt() != block->get_originalPointCnt()){
                        //ERROR, animated mesh has different pointcount than vertexAnimAtionSet
                    }
                }
                else{
                    //ERROR, geometry has no pointCnt set
                }
            }
        }
        delete targetGeosList;
        delete itMeshes;
        return NULL;*/
        delete targetGeosList;
    }
    else{
        delete targetGeosList;
        animSet->set_is_processed(true);
        char * sourceID = animSet->get_sourcePreID();
        SkeletonCacheItem * skeletonCacheItem=NULL;
        INode * sourceINode=NULL;
        AWDTriGeom * awdGeom=NULL;
        if ((animType==ANIMTYPESKELETON)||(animType==ANIMTYPEVERTEX)){
            if (sourceID==NULL){
                AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AnimationSet could not find the awd-sourceID for this AnimSet");
                awd->get_message_blocks()->append(newWarning);
                animSet->make_invalide();
                return animSet;
            }
        }
        if (animType==ANIMTYPESKELETON){
            skeletonCacheItem=skeletonCache->GetByAWDID(sourceID);
            if (skeletonCacheItem==NULL){
                AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AnimationSet could not find the AWDSkeleton");
                awd->get_message_blocks()->append(newWarning);
                animSet->make_invalide();
                return animSet;
            }
            else{
                animSet->set_skeleton(skeletonCacheItem->awdSkel);
            }
        }
        else if (animType==ANIMTYPEVERTEX){
            awdGeom=(AWDTriGeom *)animSet->get_target_geom();
            if (awdGeom == NULL){
                AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AnimationSet could not find the AWDGeometry");
                awd->get_message_blocks()->append(newWarning);
                animSet->make_invalide();
                return animSet;
            }
        }
        AWDBlockIterator *it;
        AWDBlockList * sourceClipList=NULL;
        AWDBlockList * newClipList = new AWDBlockList();
        // in simple mode we just get all awdclips, that are found on the source-obj (TODO: incl awdClips that are found on clones of sourceOBJ)
        if (animSet->get_simple_mode()){
            if ((animType==ANIMTYPESKELETON)||(animType==ANIMTYPEVERTEX)){
                sourceClipList = (AWDBlockList *)animSourceForAnimSet->Get(sourceID);
                if((sourceClipList==NULL)||(sourceClipList->get_num_blocks()==0)){
                    AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AnimationSet could not find Clip-list, or the ClipList is empty. Will not be exported");
                    awd->get_message_blocks()->append(newWarning);
                    animSet->make_invalide();
                    return animSet;
                }
            }
            if (animType==ANIMTYPESKELETON){
                AWDBlock *block;
                it = new AWDBlockIterator(sourceClipList);
                while ((block = it->next()) != NULL) {
                    AWDBlock * newClipBlock;
                    newClipBlock = ExportAWDSkeletonClip(skeletonCacheItem, (AWDSkeletonAnimation*)block);
                    if (newClipBlock!=NULL)
                        newClipList->append(newClipBlock);
                }
                delete it;
            }
            else if (animType==ANIMTYPEVERTEX){
                AWDVertexAnimation *block;
                it = new AWDBlockIterator(sourceClipList);
                while ((block = (AWDVertexAnimation *)it->next()) != NULL) {
                    AWDBlock * newClipBlock;
                    newClipBlock = ExportAWDVertexClip(awdGeom, block, blockSettings);
                    if (newClipBlock!=NULL)
                        newClipList->append(newClipBlock);
                }
                delete it;
            }
            else if (animType==ANIMTYPEUV){
                sourceClipList = animSet->get_preAnimationClipNodes();
                if((sourceClipList==NULL)||(sourceClipList->get_num_blocks()==0)){
                    AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AutoCreated-AnimationSet could not find Clip-list, or the ClipList is empty. Will not be exported");
                    awd->get_message_blocks()->append(newWarning);
                    animSet->make_invalide();
                    return animSet;
                }
                AWDUVAnimation *block;
                it = new AWDBlockIterator(sourceClipList);
                while ((block = (AWDUVAnimation *)it->next()) != NULL) {
                    AWDBlock * newClipBlock;
                    newClipBlock = ExportUVClip(block);
                    if (newClipBlock!=NULL)
                        newClipList->append(newClipBlock);
                }
                delete it;
            }
        }
        else{
            // not simple mode
            // we have read a list of animclip-blocks earlier.
            // this animclipsblocks are only spaceholders,
            // to provide a AWDID for the clip (stored as clipname)
            // and a AWDID for the source (stored as sourceID)

            sourceClipList = animSet->get_preAnimationClipNodes();
            if((sourceClipList==NULL)||(sourceClipList->get_num_blocks()==0)){
                AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AnimationSet could not find Clip-list, or the ClipList is empty. Will not be exported");
                awd->get_message_blocks()->append(newWarning);
                animSet->make_invalide();
                return animSet;
            }
            AWDVertexAnimation *block;
            it = new AWDBlockIterator(sourceClipList);
            while ((block = (AWDVertexAnimation *)it->next()) != NULL) {
                AWDBlock * newClipBlock;
                char * clipID=block->get_name();
                char * thisSourceID=block->get_sourceID();
                if((clipID==NULL)||(thisSourceID==NULL)) {
                    AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AnimationSet could not correctly read a clip from the clipslist.");
                    awd->get_message_blocks()->append(newWarning);
                }
                else{
                    if (animType==ANIMTYPESKELETON){
                        AWDSkeletonAnimation* skelAnimBlock=(AWDSkeletonAnimation *)animClipsIDsCache->Get(clipID);
                        if (skelAnimBlock==NULL){
                            AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AnimationSet could not find clip and source for a clip in the cliplist.");
                            awd->get_message_blocks()->append(newWarning);
                        }
                        else{
                            AWDBlock * newClipBlock;
                            newClipBlock = ExportAWDSkeletonClip(skeletonCacheItem, (AWDSkeletonAnimation*)skelAnimBlock);
                            if (newClipBlock!=NULL)
                                newClipList->append(newClipBlock);
                        }
                    }
                    else if (animType==ANIMTYPEVERTEX){
                        AWDVertexAnimation* vertexAnimBlock=(AWDVertexAnimation*)animClipsIDsCache->Get(clipID);
                        if (vertexAnimBlock==NULL){
                            AWDMessageBlock * newWarning = new AWDMessageBlock(animSet->get_name(), "- AnimationSet could not find clip and source for a clip in the cliplist.");
                            awd->get_message_blocks()->append(newWarning);
                        }
                        else{
                            AWDBlock * newClipBlock;
                            newClipBlock = ExportAWDVertexClip(awdGeom, vertexAnimBlock, blockSettings);
                            if (newClipBlock!=NULL)
                                newClipList->append(newClipBlock);
                        }
                    }
                    else if (animType==ANIMTYPEUV){
                        //TODO:handle UVAnim for "Not simple" mode
                        //AWDUVAnimation* uvAnimBlock=animClipsIDsCache->Get(thisSourceID);
                    }
                }
                delete block;
            }
            delete it;
        }
        if (newClipList->get_num_blocks()>0){
            animSet->set_animationClipNodes(newClipList);
            return animSet;
        }
        else
            delete newClipList;
    }
    return NULL;
}

void MaxAWDExporter::CopyViewerHTML(char *templatePath, char *outPath, char *name)
{
    char *buf;
    int bufLen;

    bufLen = 0xffff;
    buf = (char *)malloc(bufLen);

    FILE *in = fopen(templatePath, "r");
    if (in!=NULL){
        bufLen = fread((void *)buf, sizeof(char), bufLen, in);
        memset((void *)(buf + bufLen), 0, 1);
        fclose(in);

        ReplaceString(buf, &bufLen, "%NAME%", name);

        char bgcolor[8];
        snprintf(bgcolor, 8, "%x", opts->PreviewBackgroundColor());
        ReplaceString(buf, &bufLen, "%COLOR%", bgcolor);

        FILE *out = fopen(outPath, "w");
        fwrite(buf, sizeof(char), bufLen, out);
        fclose(out);
    }

    free(buf);
}

// ---> functions called by ProcessSceneGraph():