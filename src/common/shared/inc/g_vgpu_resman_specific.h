/************************************************************************************
 * ################### THIS FILE IS AUTOGENERATED. DO NOT EDIT. ################### *
 ***********************************************************************************/


#if defined (__VGPU_ALIAS_PGPU_LIST__)
static inline void _get_chip_id_for_alias_pgpu(NvU32 *dev_id, NvU32 *subdev_id)
{
    struct vgpu_alias_details {
        NvU32 alias_devid;
        NvU32 alias_subdevid;
        NvU32 devid;
        NvU32 subdevid;
    } vgpu_aliases[] = {
        { 0x20B5, 0x1642, 0x20B5, 0x1533 },
        { 0x20B8, 0x1581, 0x20B5, 0x1533 },
        { 0x20B7, 0x1804, 0x20B7, 0x1532 },
        { 0x20B9, 0x157F, 0x20B7, 0x1532 },
        { 0x20FD, 0x17F8, 0x20F5, 0x0 },
        { 0x2324, 0x17A8, 0x2324, 0x17A6 },
        { 0x2329, 0x198C, 0x2329, 0x198B },
        { 0x232C, 0x2064, 0x232C, 0x2063 },
        { 0x2330, 0x16C0, 0x2330, 0x16C1 },
        { 0x2336, 0x16C2, 0x2330, 0x16C1 },
        { 0x2335, 0x18BF, 0x2335, 0x18BE },
        { 0x26BA, 0x1990, 0x26BA, 0x1957 },
        { 0x2941, 0x2046, 0x2941, 0x2045 },
    };

    for (NvU32 i = 0; i < (sizeof(vgpu_aliases) / sizeof(struct vgpu_alias_details)); ++i) {
        if ((*dev_id == vgpu_aliases[i].alias_devid) && (*subdev_id == vgpu_aliases[i].alias_subdevid)) {
            *dev_id = vgpu_aliases[i].devid;
            *subdev_id = vgpu_aliases[i].subdevid;
            break;
        }
    }

    return;
}
#endif //__VGPU_ALIAS_PGPU_LIST__


#if defined(GENERATE_vgpuSmcTypeIdMappings)
static const struct {
    NvU32 devId;
    NvU32 partitionFlag;
    NvU32 vgpuTypeId;
} vgpuSmcTypeIdMappings [] = {
    {0x20B010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     ,  463}, // GRID A100X-1-5C
    {0x20B010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  464}, // GRID A100X-2-10C
    {0x20B010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    ,  465}, // GRID A100X-3-20C
    {0x20B010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  466}, // GRID A100X-4-20C
    {0x20B010DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  467}, // GRID A100X-7-40C
    {0x20B010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE),  704}, // GRID A100X-1-5CME
    {0x20B010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1056}, // GRID A100X-1-10C
    {0x20B210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     ,  622}, // GRID A100DX-1-10C
    {0x20B210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  623}, // GRID A100DX-2-20C
    {0x20B210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    ,  624}, // GRID A100DX-3-40C
    {0x20B210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  625}, // GRID A100DX-4-40C
    {0x20B210DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  626}, // GRID A100DX-7-80C
    {0x20B210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE),  705}, // GRID A100DX-1-10CME
    {0x20B210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1057}, // GRID A100DX-1-20C
    {0x20B510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     ,  699}, // GRID A100D-1-10C
    {0x20B810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     ,  699}, // GRID A100D-1-10C
    {0x20B510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  700}, // GRID A100D-2-20C
    {0x20B810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  700}, // GRID A100D-2-20C
    {0x20B510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    ,  701}, // GRID A100D-3-40C
    {0x20B810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    ,  701}, // GRID A100D-3-40C
    {0x20B510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  702}, // GRID A100D-4-40C
    {0x20B810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  702}, // GRID A100D-4-40C
    {0x20B510DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  703}, // GRID A100D-7-80C
    {0x20B810DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  703}, // GRID A100D-7-80C
    {0x20B510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE),  707}, // GRID A100D-1-10CME
    {0x20B810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE),  707}, // GRID A100D-1-10CME
    {0x20B510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1054}, // GRID A100D-1-20C
    {0x20B810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1054}, // GRID A100D-1-20C
    {0x20B710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  689}, // NVIDIA A30-1-6C
    {0x20B910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  689}, // NVIDIA A30-1-6C
    {0x20B710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  690}, // NVIDIA A30-2-12C
    {0x20B910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  690}, // NVIDIA A30-2-12C
    {0x20B710DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  691}, // NVIDIA A30-4-24C
    {0x20B910DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  691}, // NVIDIA A30-4-24C
    {0x20B710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE) ,  708}, // NVIDIA A30-1-6CME
    {0x20B910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE) ,  708}, // NVIDIA A30-1-6CME
    {0x20B710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE)    , 1052}, // NVIDIA A30-2-12CME
    {0x20B910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE)    , 1052}, // NVIDIA A30-2-12CME
    {0x20F110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     ,  474}, // GRID A100-1-5C
    {0x20F110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  475}, // GRID A100-2-10C
    {0x20F110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    ,  476}, // GRID A100-3-20C
    {0x20F110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  477}, // GRID A100-4-20C
    {0x20F110DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  478}, // GRID A100-7-40C
    {0x20F110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE),  706}, // GRID A100-1-5CME
    {0x20F110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1053}, // GRID A100-1-10C
    {0x20F310DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE),  994}, // GRID A800DX-1-10CME
    {0x20F310DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     ,  995}, // GRID A800DX-1-10C
    {0x20F310DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  996}, // GRID A800DX-2-20C
    {0x20F310DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    ,  997}, // GRID A800DX-3-40C
    {0x20F310DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  998}, // GRID A800DX-4-40C
    {0x20F310DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  999}, // GRID A800DX-7-80C
    {0x20F310DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1058}, // GRID A800DX-1-20C
    {0x20F510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1007}, // GRID A800D-1-10CME
    {0x20FD10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1007}, // GRID A800D-1-10CME
    {0x20F510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1008}, // GRID A800D-1-10C
    {0x20FD10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1008}, // GRID A800D-1-10C
    {0x20F510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1009}, // GRID A800D-2-20C
    {0x20FD10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1009}, // GRID A800D-2-20C
    {0x20F510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1010}, // GRID A800D-3-40C
    {0x20FD10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1010}, // GRID A800D-3-40C
    {0x20F510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1011}, // GRID A800D-4-40C
    {0x20FD10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1011}, // GRID A800D-4-40C
    {0x20F510DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1012}, // GRID A800D-7-80C
    {0x20FD10DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1012}, // GRID A800D-7-80C
    {0x20F510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1055}, // GRID A800D-1-20C
    {0x20FD10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1055}, // GRID A800D-1-20C
    {0x20F610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1089}, // GRID A800-1-5CME
    {0x20F610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1090}, // GRID A800-1-5C
    {0x20F610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1092}, // GRID A800-2-10C
    {0x20F610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1093}, // GRID A800-3-20C
    {0x20F610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1094}, // GRID A800-4-20C
    {0x20F610DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1095}, // GRID A800-7-40C
    {0x20F610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1091}, // GRID A800-1-10C
    {0x232110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1061}, // NVIDIA H100L-1-12CME
    {0x232110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1062}, // NVIDIA H100L-1-12C
    {0x232110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1063}, // NVIDIA H100L-1-24C
    {0x232110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1064}, // NVIDIA H100L-2-24C
    {0x232110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1065}, // NVIDIA H100L-3-47C
    {0x232110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1066}, // NVIDIA H100L-4-47C
    {0x232110DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1067}, // NVIDIA H100L-7-94C
    {0x232210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE),  979}, // NVIDIA H800-1-10CME
    {0x232210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     ,  980}, // NVIDIA H800-1-10C
    {0x232210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  981}, // NVIDIA H800-2-20C
    {0x232210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    ,  982}, // NVIDIA H800-3-40C
    {0x232210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  983}, // NVIDIA H800-4-40C
    {0x232210DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  984}, // NVIDIA H800-7-80C
    {0x232210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1060}, // NVIDIA H800-1-20C
    {0x232410DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1276}, // NVIDIA H800XM-1-10CME
    {0x232410DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1277}, // NVIDIA H800XM-1-10C
    {0x232410DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1278}, // NVIDIA H800XM-1-20C
    {0x232410DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1279}, // NVIDIA H800XM-2-20C
    {0x232410DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1280}, // NVIDIA H800XM-3-40C
    {0x232410DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1281}, // NVIDIA H800XM-4-40C
    {0x232410DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1282}, // NVIDIA H800XM-7-80C
    {0x232910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1397}, // NVIDIA H20-1-12CME
    {0x232910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1398}, // NVIDIA H20-1-12C
    {0x232910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1399}, // NVIDIA H20-1-24C
    {0x232910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1400}, // NVIDIA H20-2-24C
    {0x232910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1401}, // NVIDIA H20-3-48C
    {0x232910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1402}, // NVIDIA H20-4-48C
    {0x232910DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1403}, // NVIDIA H20-7-96C
    {0x232C10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1480}, // NVIDIA H20X-1-18CME
    {0x232C10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1481}, // NVIDIA H20X-1-18C
    {0x232C10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1482}, // NVIDIA H20X-1-35C
    {0x232C10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1483}, // NVIDIA H20X-2-35C
    {0x232C10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1484}, // NVIDIA H20X-3-71C
    {0x232C10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1485}, // NVIDIA H20X-4-71C
    {0x232C10DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1486}, // NVIDIA H20X-7-141C
    {0x233010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1130}, // NVIDIA H100XM-1-10CME
    {0x233610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1130}, // NVIDIA H100XM-1-10CME
    {0x233010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1131}, // NVIDIA H100XM-1-10C
    {0x233610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1131}, // NVIDIA H100XM-1-10C
    {0x233010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1132}, // NVIDIA H100XM-1-20C
    {0x233610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1132}, // NVIDIA H100XM-1-20C
    {0x233010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1133}, // NVIDIA H100XM-2-20C
    {0x233610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1133}, // NVIDIA H100XM-2-20C
    {0x233010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1134}, // NVIDIA H100XM-3-40C
    {0x233610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1134}, // NVIDIA H100XM-3-40C
    {0x233010DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1135}, // NVIDIA H100XM-4-40C
    {0x233610DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1135}, // NVIDIA H100XM-4-40C
    {0x233010DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1136}, // NVIDIA H100XM-7-80C
    {0x233610DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1136}, // NVIDIA H100XM-7-80C
    {0x233110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     ,  821}, // NVIDIA H100-1-10C
    {0x233110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      ,  822}, // NVIDIA H100-2-20C
    {0x233110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    ,  823}, // NVIDIA H100-3-40C
    {0x233110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         ,  824}, // NVIDIA H100-4-40C
    {0x233110DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             ,  825}, // NVIDIA H100-7-80C
    {0x233110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE),  993}, // NVIDIA H100-1-10CME
    {0x233110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1059}, // NVIDIA H100-1-20C
    {0x233510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1413}, // NVIDIA H200X-1-18CME
    {0x233510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1414}, // NVIDIA H200X-1-18C
    {0x233510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1415}, // NVIDIA H200X-1-35C
    {0x233510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1416}, // NVIDIA H200X-2-35C
    {0x233510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1417}, // NVIDIA H200X-3-71C
    {0x233510DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1418}, // NVIDIA H200X-4-71C
    {0x233510DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1419}, // NVIDIA H200X-7-141C
    {0x233710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1305}, // NVIDIA H100XS-1-8CME
    {0x233710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1306}, // NVIDIA H100XS-1-8C
    {0x233710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1307}, // NVIDIA H100XS-1-16C
    {0x233710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1308}, // NVIDIA H100XS-2-16C
    {0x233710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1309}, // NVIDIA H100XS-3-32C
    {0x233710DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1310}, // NVIDIA H100XS-4-32C
    {0x233710DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1311}, // NVIDIA H100XS-7-64C
    {0x233910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1291}, // NVIDIA H100XL-1-12CME
    {0x233910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1292}, // NVIDIA H100XL-1-12C
    {0x233910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1293}, // NVIDIA H100XL-1-24C
    {0x233910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1294}, // NVIDIA H100XL-2-24C
    {0x233910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1295}, // NVIDIA H100XL-3-47C
    {0x233910DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1296}, // NVIDIA H100XL-4-47C
    {0x233910DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1297}, // NVIDIA H100XL-7-94C
    {0x233A10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1075}, // NVIDIA H800L-1-12CME
    {0x233A10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1076}, // NVIDIA H800L-1-12C
    {0x233A10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1077}, // NVIDIA H800L-1-24C
    {0x233A10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1078}, // NVIDIA H800L-2-24C
    {0x233A10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1079}, // NVIDIA H800L-3-47C
    {0x233A10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1080}, // NVIDIA H800L-4-47C
    {0x233A10DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1081}, // NVIDIA H800L-7-94C
    {0x233B10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1429}, // NVIDIA H200-1-18CME
    {0x233B10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1430}, // NVIDIA H200-1-18C
    {0x233B10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1431}, // NVIDIA H200-1-35C
    {0x233B10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1432}, // NVIDIA H200-2-35C
    {0x233B10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1433}, // NVIDIA H200-3-71C
    {0x233B10DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1434}, // NVIDIA H200-4-71C
    {0x233B10DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1435}, // NVIDIA H200-7-141C
    {0x234210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1196}, // NVIDIA GH200-1-12CME
    {0x234210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1197}, // NVIDIA GH200-1-12C
    {0x234210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1198}, // NVIDIA GH200-1-24C
    {0x234210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1199}, // NVIDIA GH200-2-24C
    {0x234210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1200}, // NVIDIA GH200-3-48C
    {0x234210DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1201}, // NVIDIA GH200-4-48C
    {0x234210DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1202}, // NVIDIA GH200-7-96C
    {0x234810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1450}, // NVIDIA GH200L-1-18CME
    {0x234810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1451}, // NVIDIA GH200L-1-18C
    {0x234810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1452}, // NVIDIA GH200L-1-36C
    {0x234810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1453}, // NVIDIA GH200L-2-36C
    {0x234810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1454}, // NVIDIA GH200L-3-72C
    {0x234810DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1455}, // NVIDIA GH200L-4-72C
    {0x234810DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1456}, // NVIDIA GH200L-7-144C
    {0x294110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU | DRF_DEF(2080, _CTRL_GPU_PARTITION_FLAG, _REQ_DEC_JPG_OFA, _ENABLE), 1445}, // NVIDIA GB200-1-24CME
    {0x294110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_EIGHTHED_GPU                                                                     , 1446}, // NVIDIA GB200-1-24C
    {0x294110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_QUARTER_GPU                                                                 , 1458}, // NVIDIA GB200-1-47C
    {0x294110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_QUARTER_GPU                                                                      , 1447}, // NVIDIA GB200-2-47C
    {0x294110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_MINI_HALF_GPU                                                                    , 1459}, // NVIDIA GB200-3-95C
    {0x294110DE, NV2080_CTRL_GPU_PARTITION_FLAG_ONE_HALF_GPU                                                                         , 1448}, // NVIDIA GB200-4-95C
    {0x294110DE, NV2080_CTRL_GPU_PARTITION_FLAG_FULL_GPU                                                                             , 1460}, // NVIDIA GB200-7-189C

};
#endif // GENERATE_vgpuSmcTypeIdMappings


