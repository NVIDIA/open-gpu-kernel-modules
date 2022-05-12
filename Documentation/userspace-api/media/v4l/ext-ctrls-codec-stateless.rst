.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _codec-stateless-controls:

*********************************
Stateless Codec Control Reference
*********************************

The Stateless Codec control class is intended to support
stateless decoder and encoders (i.e. hardware accelerators).

These drivers are typically supported by the :ref:`stateless_decoder`,
and deal with parsed pixel formats such as V4L2_PIX_FMT_H264_SLICE.

Stateless Codec Control ID
==========================

.. _codec-stateless-control-id:

``V4L2_CID_CODEC_STATELESS_CLASS (class)``
    The Stateless Codec class descriptor.

.. _v4l2-codec-stateless-h264:

``V4L2_CID_STATELESS_H264_SPS (struct)``
    Specifies the sequence parameter set (as extracted from the
    bitstream) for the associated H264 slice data. This includes the
    necessary parameters for configuring a stateless hardware decoding
    pipeline for H264. The bitstream parameters are defined according
    to :ref:`h264`, section 7.4.2.1.1 "Sequence Parameter Set Data
    Semantics". For further documentation, refer to the above
    specification, unless there is an explicit comment stating
    otherwise.

.. c:type:: v4l2_ctrl_h264_sps

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.2cm}|p{8.6cm}|p{7.5cm}|

.. flat-table:: struct v4l2_ctrl_h264_sps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``profile_idc``
      -
    * - __u8
      - ``constraint_set_flags``
      - See :ref:`Sequence Parameter Set Constraints Set Flags <h264_sps_constraints_set_flags>`
    * - __u8
      - ``level_idc``
      -
    * - __u8
      - ``seq_parameter_set_id``
      -
    * - __u8
      - ``chroma_format_idc``
      -
    * - __u8
      - ``bit_depth_luma_minus8``
      -
    * - __u8
      - ``bit_depth_chroma_minus8``
      -
    * - __u8
      - ``log2_max_frame_num_minus4``
      -
    * - __u8
      - ``pic_order_cnt_type``
      -
    * - __u8
      - ``log2_max_pic_order_cnt_lsb_minus4``
      -
    * - __u8
      - ``max_num_ref_frames``
      -
    * - __u8
      - ``num_ref_frames_in_pic_order_cnt_cycle``
      -
    * - __s32
      - ``offset_for_ref_frame[255]``
      -
    * - __s32
      - ``offset_for_non_ref_pic``
      -
    * - __s32
      - ``offset_for_top_to_bottom_field``
      -
    * - __u16
      - ``pic_width_in_mbs_minus1``
      -
    * - __u16
      - ``pic_height_in_map_units_minus1``
      -
    * - __u32
      - ``flags``
      - See :ref:`Sequence Parameter Set Flags <h264_sps_flags>`

.. raw:: latex

    \normalsize

.. _h264_sps_constraints_set_flags:

``Sequence Parameter Set Constraints Set Flags``

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_SPS_CONSTRAINT_SET0_FLAG``
      - 0x00000001
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET1_FLAG``
      - 0x00000002
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET2_FLAG``
      - 0x00000004
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET3_FLAG``
      - 0x00000008
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET4_FLAG``
      - 0x00000010
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET5_FLAG``
      - 0x00000020
      -

.. _h264_sps_flags:

``Sequence Parameter Set Flags``

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_SPS_FLAG_SEPARATE_COLOUR_PLANE``
      - 0x00000001
      -
    * - ``V4L2_H264_SPS_FLAG_QPPRIME_Y_ZERO_TRANSFORM_BYPASS``
      - 0x00000002
      -
    * - ``V4L2_H264_SPS_FLAG_DELTA_PIC_ORDER_ALWAYS_ZERO``
      - 0x00000004
      -
    * - ``V4L2_H264_SPS_FLAG_GAPS_IN_FRAME_NUM_VALUE_ALLOWED``
      - 0x00000008
      -
    * - ``V4L2_H264_SPS_FLAG_FRAME_MBS_ONLY``
      - 0x00000010
      -
    * - ``V4L2_H264_SPS_FLAG_MB_ADAPTIVE_FRAME_FIELD``
      - 0x00000020
      -
    * - ``V4L2_H264_SPS_FLAG_DIRECT_8X8_INFERENCE``
      - 0x00000040
      -

``V4L2_CID_STATELESS_H264_PPS (struct)``
    Specifies the picture parameter set (as extracted from the
    bitstream) for the associated H264 slice data. This includes the
    necessary parameters for configuring a stateless hardware decoding
    pipeline for H264.  The bitstream parameters are defined according
    to :ref:`h264`, section 7.4.2.2 "Picture Parameter Set RBSP
    Semantics". For further documentation, refer to the above
    specification, unless there is an explicit comment stating
    otherwise.

.. c:type:: v4l2_ctrl_h264_pps

.. raw:: latex

    \small

.. flat-table:: struct v4l2_ctrl_h264_pps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``pic_parameter_set_id``
      -
    * - __u8
      - ``seq_parameter_set_id``
      -
    * - __u8
      - ``num_slice_groups_minus1``
      -
    * - __u8
      - ``num_ref_idx_l0_default_active_minus1``
      -
    * - __u8
      - ``num_ref_idx_l1_default_active_minus1``
      -
    * - __u8
      - ``weighted_bipred_idc``
      -
    * - __s8
      - ``pic_init_qp_minus26``
      -
    * - __s8
      - ``pic_init_qs_minus26``
      -
    * - __s8
      - ``chroma_qp_index_offset``
      -
    * - __s8
      - ``second_chroma_qp_index_offset``
      -
    * - __u16
      - ``flags``
      - See :ref:`Picture Parameter Set Flags <h264_pps_flags>`

.. raw:: latex

    \normalsize

.. _h264_pps_flags:

``Picture Parameter Set Flags``

.. raw:: latex

    \begingroup
    \scriptsize
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{9.8cm}|p{1.0cm}|p{6.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       10 1 4

    * - ``V4L2_H264_PPS_FLAG_ENTROPY_CODING_MODE``
      - 0x0001
      -
    * - ``V4L2_H264_PPS_FLAG_BOTTOM_FIELD_PIC_ORDER_IN_FRAME_PRESENT``
      - 0x0002
      -
    * - ``V4L2_H264_PPS_FLAG_WEIGHTED_PRED``
      - 0x0004
      -
    * - ``V4L2_H264_PPS_FLAG_DEBLOCKING_FILTER_CONTROL_PRESENT``
      - 0x0008
      -
    * - ``V4L2_H264_PPS_FLAG_CONSTRAINED_INTRA_PRED``
      - 0x0010
      -
    * - ``V4L2_H264_PPS_FLAG_REDUNDANT_PIC_CNT_PRESENT``
      - 0x0020
      -
    * - ``V4L2_H264_PPS_FLAG_TRANSFORM_8X8_MODE``
      - 0x0040
      -
    * - ``V4L2_H264_PPS_FLAG_SCALING_MATRIX_PRESENT``
      - 0x0080
      - ``V4L2_CID_STATELESS_H264_SCALING_MATRIX``
        must be used for this picture.

.. raw:: latex

    \endgroup

``V4L2_CID_STATELESS_H264_SCALING_MATRIX (struct)``
    Specifies the scaling matrix (as extracted from the bitstream) for
    the associated H264 slice data. The bitstream parameters are
    defined according to :ref:`h264`, section 7.4.2.1.1.1 "Scaling
    List Semantics". For further documentation, refer to the above
    specification, unless there is an explicit comment stating
    otherwise.

.. c:type:: v4l2_ctrl_h264_scaling_matrix

.. raw:: latex

    \small

.. tabularcolumns:: |p{0.6cm}|p{4.8cm}|p{11.9cm}|

.. flat-table:: struct v4l2_ctrl_h264_scaling_matrix
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``scaling_list_4x4[6][16]``
      - Scaling matrix after applying the inverse scanning process.
        Expected list order is Intra Y, Intra Cb, Intra Cr, Inter Y,
        Inter Cb, Inter Cr. The values on each scaling list are
        expected in raster scan order.
    * - __u8
      - ``scaling_list_8x8[6][64]``
      - Scaling matrix after applying the inverse scanning process.
        Expected list order is Intra Y, Inter Y, Intra Cb, Inter Cb,
        Intra Cr, Inter Cr. The values on each scaling list are
        expected in raster scan order.

``V4L2_CID_STATELESS_H264_SLICE_PARAMS (struct)``
    Specifies the slice parameters (as extracted from the bitstream)
    for the associated H264 slice data. This includes the necessary
    parameters for configuring a stateless hardware decoding pipeline
    for H264.  The bitstream parameters are defined according to
    :ref:`h264`, section 7.4.3 "Slice Header Semantics". For further
    documentation, refer to the above specification, unless there is
    an explicit comment stating otherwise.

.. c:type:: v4l2_ctrl_h264_slice_params

.. raw:: latex

    \small

.. tabularcolumns:: |p{4.0cm}|p{5.9cm}|p{7.4cm}|

.. flat-table:: struct v4l2_ctrl_h264_slice_params
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``header_bit_size``
      - Offset in bits to slice_data() from the beginning of this slice.
    * - __u32
      - ``first_mb_in_slice``
      -
    * - __u8
      - ``slice_type``
      -
    * - __u8
      - ``colour_plane_id``
      -
    * - __u8
      - ``redundant_pic_cnt``
      -
    * - __u8
      - ``cabac_init_idc``
      -
    * - __s8
      - ``slice_qp_delta``
      -
    * - __s8
      - ``slice_qs_delta``
      -
    * - __u8
      - ``disable_deblocking_filter_idc``
      -
    * - __s8
      - ``slice_alpha_c0_offset_div2``
      -
    * - __s8
      - ``slice_beta_offset_div2``
      -
    * - __u8
      - ``num_ref_idx_l0_active_minus1``
      - If num_ref_idx_active_override_flag is not set, this field must be
        set to the value of num_ref_idx_l0_default_active_minus1
    * - __u8
      - ``num_ref_idx_l1_active_minus1``
      - If num_ref_idx_active_override_flag is not set, this field must be
        set to the value of num_ref_idx_l1_default_active_minus1
    * - __u8
      - ``reserved``
      - Applications and drivers must set this to zero.
    * - struct :c:type:`v4l2_h264_reference`
      - ``ref_pic_list0[32]``
      - Reference picture list after applying the per-slice modifications
    * - struct :c:type:`v4l2_h264_reference`
      - ``ref_pic_list1[32]``
      - Reference picture list after applying the per-slice modifications
    * - __u32
      - ``flags``
      - See :ref:`Slice Parameter Flags <h264_slice_flags>`

.. raw:: latex

    \normalsize

.. _h264_slice_flags:

``Slice Parameter Set Flags``

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_SLICE_FLAG_DIRECT_SPATIAL_MV_PRED``
      - 0x00000001
      -
    * - ``V4L2_H264_SLICE_FLAG_SP_FOR_SWITCH``
      - 0x00000002
      -

``V4L2_CID_STATELESS_H264_PRED_WEIGHTS (struct)``
    Prediction weight table defined according to :ref:`h264`,
    section 7.4.3.2 "Prediction Weight Table Semantics".
    The prediction weight table must be passed by applications
    under the conditions explained in section 7.3.3 "Slice header
    syntax".

.. c:type:: v4l2_ctrl_h264_pred_weights

.. raw:: latex

    \small

.. tabularcolumns:: |p{4.9cm}|p{4.9cm}|p{7.5cm}|

.. flat-table:: struct v4l2_ctrl_h264_pred_weights
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u16
      - ``luma_log2_weight_denom``
      -
    * - __u16
      - ``chroma_log2_weight_denom``
      -
    * - struct :c:type:`v4l2_h264_weight_factors`
      - ``weight_factors[2]``
      - The weight factors at index 0 are the weight factors for the reference
        list 0, the one at index 1 for the reference list 1.

.. raw:: latex

    \normalsize

.. c:type:: v4l2_h264_weight_factors

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.0cm}|p{4.5cm}|p{11.8cm}|

.. flat-table:: struct v4l2_h264_weight_factors
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s16
      - ``luma_weight[32]``
      -
    * - __s16
      - ``luma_offset[32]``
      -
    * - __s16
      - ``chroma_weight[32][2]``
      -
    * - __s16
      - ``chroma_offset[32][2]``
      -

.. raw:: latex

    \normalsize

``Picture Reference``

.. c:type:: v4l2_h264_reference

.. cssclass:: longtable

.. flat-table:: struct v4l2_h264_reference
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``fields``
      - Specifies how the picture is referenced. See :ref:`Reference Fields <h264_ref_fields>`
    * - __u8
      - ``index``
      - Index into the :c:type:`v4l2_ctrl_h264_decode_params`.dpb array.

.. _h264_ref_fields:

``Reference Fields``

.. raw:: latex

    \small

.. tabularcolumns:: |p{5.4cm}|p{0.8cm}|p{11.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_TOP_FIELD_REF``
      - 0x1
      - The top field in field pair is used for short-term reference.
    * - ``V4L2_H264_BOTTOM_FIELD_REF``
      - 0x2
      - The bottom field in field pair is used for short-term reference.
    * - ``V4L2_H264_FRAME_REF``
      - 0x3
      - The frame (or the top/bottom fields, if it's a field pair)
        is used for short-term reference.

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_H264_DECODE_PARAMS (struct)``
    Specifies the decode parameters (as extracted from the bitstream)
    for the associated H264 slice data. This includes the necessary
    parameters for configuring a stateless hardware decoding pipeline
    for H264. The bitstream parameters are defined according to
    :ref:`h264`. For further documentation, refer to the above
    specification, unless there is an explicit comment stating
    otherwise.

.. c:type:: v4l2_ctrl_h264_decode_params

.. raw:: latex

    \small

.. tabularcolumns:: |p{4.0cm}|p{5.9cm}|p{7.4cm}|

.. flat-table:: struct v4l2_ctrl_h264_decode_params
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct :c:type:`v4l2_h264_dpb_entry`
      - ``dpb[16]``
      -
    * - __u16
      - ``nal_ref_idc``
      - NAL reference ID value coming from the NAL Unit header
    * - __u16
      - ``frame_num``
      -
    * - __s32
      - ``top_field_order_cnt``
      - Picture Order Count for the coded top field
    * - __s32
      - ``bottom_field_order_cnt``
      - Picture Order Count for the coded bottom field
    * - __u16
      - ``idr_pic_id``
      -
    * - __u16
      - ``pic_order_cnt_lsb``
      -
    * - __s32
      - ``delta_pic_order_cnt_bottom``
      -
    * - __s32
      - ``delta_pic_order_cnt0``
      -
    * - __s32
      - ``delta_pic_order_cnt1``
      -
    * - __u32
      - ``dec_ref_pic_marking_bit_size``
      - Size in bits of the dec_ref_pic_marking() syntax element.
    * - __u32
      - ``pic_order_cnt_bit_size``
      - Combined size in bits of the picture order count related syntax
        elements: pic_order_cnt_lsb, delta_pic_order_cnt_bottom,
        delta_pic_order_cnt0, and delta_pic_order_cnt1.
    * - __u32
      - ``slice_group_change_cycle``
      -
    * - __u32
      - ``reserved``
      - Applications and drivers must set this to zero.
    * - __u32
      - ``flags``
      - See :ref:`Decode Parameters Flags <h264_decode_params_flags>`

.. raw:: latex

    \normalsize

.. _h264_decode_params_flags:

``Decode Parameters Flags``

.. raw:: latex

    \small

.. tabularcolumns:: |p{8.3cm}|p{2.1cm}|p{6.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_DECODE_PARAM_FLAG_IDR_PIC``
      - 0x00000001
      - That picture is an IDR picture
    * - ``V4L2_H264_DECODE_PARAM_FLAG_FIELD_PIC``
      - 0x00000002
      -
    * - ``V4L2_H264_DECODE_PARAM_FLAG_BOTTOM_FIELD``
      - 0x00000004
      -

.. raw:: latex

    \normalsize

.. c:type:: v4l2_h264_dpb_entry

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.0cm}|p{4.9cm}|p{11.4cm}|

.. flat-table:: struct v4l2_h264_dpb_entry
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u64
      - ``reference_ts``
      - Timestamp of the V4L2 capture buffer to use as reference, used
        with B-coded and P-coded frames. The timestamp refers to the
        ``timestamp`` field in struct :c:type:`v4l2_buffer`. Use the
        :c:func:`v4l2_timeval_to_ns()` function to convert the struct
        :c:type:`timeval` in struct :c:type:`v4l2_buffer` to a __u64.
    * - __u32
      - ``pic_num``
      -
    * - __u16
      - ``frame_num``
      -
    * - __u8
      - ``fields``
      - Specifies how the DPB entry is referenced. See :ref:`Reference Fields <h264_ref_fields>`
    * - __u8
      - ``reserved[5]``
      - Applications and drivers must set this to zero.
    * - __s32
      - ``top_field_order_cnt``
      -
    * - __s32
      - ``bottom_field_order_cnt``
      -
    * - __u32
      - ``flags``
      - See :ref:`DPB Entry Flags <h264_dpb_flags>`

.. raw:: latex

    \normalsize

.. _h264_dpb_flags:

``DPB Entries Flags``

.. raw:: latex

    \small

.. tabularcolumns:: |p{7.7cm}|p{2.1cm}|p{7.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_DPB_ENTRY_FLAG_VALID``
      - 0x00000001
      - The DPB entry is valid (non-empty) and should be considered.
    * - ``V4L2_H264_DPB_ENTRY_FLAG_ACTIVE``
      - 0x00000002
      - The DPB entry is used for reference.
    * - ``V4L2_H264_DPB_ENTRY_FLAG_LONG_TERM``
      - 0x00000004
      - The DPB entry is used for long-term reference.
    * - ``V4L2_H264_DPB_ENTRY_FLAG_FIELD``
      - 0x00000008
      - The DPB entry is a single field or a complementary field pair.

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_H264_DECODE_MODE (enum)``
    Specifies the decoding mode to use. Currently exposes slice-based and
    frame-based decoding but new modes might be added later on.
    This control is used as a modifier for V4L2_PIX_FMT_H264_SLICE
    pixel format. Applications that support V4L2_PIX_FMT_H264_SLICE
    are required to set this control in order to specify the decoding mode
    that is expected for the buffer.
    Drivers may expose a single or multiple decoding modes, depending
    on what they can support.

.. c:type:: v4l2_stateless_h264_decode_mode

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_STATELESS_H264_DECODE_MODE_SLICE_BASED``
      - 0
      - Decoding is done at the slice granularity.
        The OUTPUT buffer must contain a single slice.
        When this mode is selected, the ``V4L2_CID_STATELESS_H264_SLICE_PARAMS``
        control shall be set. When multiple slices compose a frame,
        use of ``V4L2_BUF_CAP_SUPPORTS_M2M_HOLD_CAPTURE_BUF`` flag
        is required.
    * - ``V4L2_STATELESS_H264_DECODE_MODE_FRAME_BASED``
      - 1
      - Decoding is done at the frame granularity,
        The OUTPUT buffer must contain all slices needed to decode the
        frame. The OUTPUT buffer must also contain both fields.
        This mode will be supported by devices that
        parse the slice(s) header(s) in hardware. When this mode is
        selected, the ``V4L2_CID_STATELESS_H264_SLICE_PARAMS``
        control shall not be set.

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_H264_START_CODE (enum)``
    Specifies the H264 slice start code expected for each slice.
    This control is used as a modifier for V4L2_PIX_FMT_H264_SLICE
    pixel format. Applications that support V4L2_PIX_FMT_H264_SLICE
    are required to set this control in order to specify the start code
    that is expected for the buffer.
    Drivers may expose a single or multiple start codes, depending
    on what they can support.

.. c:type:: v4l2_stateless_h264_start_code

.. raw:: latex

    \small

.. tabularcolumns:: |p{7.9cm}|p{0.4cm}|p{9.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       4 1 4

    * - ``V4L2_STATELESS_H264_START_CODE_NONE``
      - 0
      - Selecting this value specifies that H264 slices are passed
        to the driver without any start code. The bitstream data should be
        according to :ref:`h264` 7.3.1 NAL unit syntax, hence contains
        emulation prevention bytes when required.
    * - ``V4L2_STATELESS_H264_START_CODE_ANNEX_B``
      - 1
      - Selecting this value specifies that H264 slices are expected
        to be prefixed by Annex B start codes. According to :ref:`h264`
        valid start codes can be 3-bytes 0x000001 or 4-bytes 0x00000001.

.. raw:: latex

    \normalsize

.. _codec-stateless-fwht:

``V4L2_CID_STATELESS_FWHT_PARAMS (struct)``
    Specifies the FWHT (Fast Walsh Hadamard Transform) parameters (as extracted
    from the bitstream) for the associated FWHT data. This includes the necessary
    parameters for configuring a stateless hardware decoding pipeline for FWHT.
    This codec is specific to the vicodec test driver.

.. c:type:: v4l2_ctrl_fwht_params

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.4cm}|p{3.9cm}|p{12.0cm}|

.. flat-table:: struct v4l2_ctrl_fwht_params
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u64
      - ``backward_ref_ts``
      - Timestamp of the V4L2 capture buffer to use as backward reference, used
        with P-coded frames. The timestamp refers to the
	``timestamp`` field in struct :c:type:`v4l2_buffer`. Use the
	:c:func:`v4l2_timeval_to_ns()` function to convert the struct
	:c:type:`timeval` in struct :c:type:`v4l2_buffer` to a __u64.
    * - __u32
      - ``version``
      - The version of the codec. Set to ``V4L2_FWHT_VERSION``.
    * - __u32
      - ``width``
      - The width of the frame.
    * - __u32
      - ``height``
      - The height of the frame.
    * - __u32
      - ``flags``
      - The flags of the frame, see :ref:`fwht-flags`.
    * - __u32
      - ``colorspace``
      - The colorspace of the frame, from enum :c:type:`v4l2_colorspace`.
    * - __u32
      - ``xfer_func``
      - The transfer function, from enum :c:type:`v4l2_xfer_func`.
    * - __u32
      - ``ycbcr_enc``
      - The Y'CbCr encoding, from enum :c:type:`v4l2_ycbcr_encoding`.
    * - __u32
      - ``quantization``
      - The quantization range, from enum :c:type:`v4l2_quantization`.

.. raw:: latex

    \normalsize

.. _fwht-flags:

FWHT Flags
==========

.. raw:: latex

    \small

.. tabularcolumns:: |p{7.0cm}|p{2.3cm}|p{8.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_FWHT_FL_IS_INTERLACED``
      - 0x00000001
      - Set if this is an interlaced format.
    * - ``V4L2_FWHT_FL_IS_BOTTOM_FIRST``
      - 0x00000002
      - Set if this is a bottom-first (NTSC) interlaced format.
    * - ``V4L2_FWHT_FL_IS_ALTERNATE``
      - 0x00000004
      - Set if each 'frame' contains just one field.
    * - ``V4L2_FWHT_FL_IS_BOTTOM_FIELD``
      - 0x00000008
      - If V4L2_FWHT_FL_IS_ALTERNATE was set, then this is set if this 'frame' is the
	bottom field, else it is the top field.
    * - ``V4L2_FWHT_FL_LUMA_IS_UNCOMPRESSED``
      - 0x00000010
      - Set if the Y' (luma) plane is uncompressed.
    * - ``V4L2_FWHT_FL_CB_IS_UNCOMPRESSED``
      - 0x00000020
      - Set if the Cb plane is uncompressed.
    * - ``V4L2_FWHT_FL_CR_IS_UNCOMPRESSED``
      - 0x00000040
      - Set if the Cr plane is uncompressed.
    * - ``V4L2_FWHT_FL_CHROMA_FULL_HEIGHT``
      - 0x00000080
      - Set if the chroma plane has the same height as the luma plane,
	else the chroma plane is half the height of the luma plane.
    * - ``V4L2_FWHT_FL_CHROMA_FULL_WIDTH``
      - 0x00000100
      - Set if the chroma plane has the same width as the luma plane,
	else the chroma plane is half the width of the luma plane.
    * - ``V4L2_FWHT_FL_ALPHA_IS_UNCOMPRESSED``
      - 0x00000200
      - Set if the alpha plane is uncompressed.
    * - ``V4L2_FWHT_FL_I_FRAME``
      - 0x00000400
      - Set if this is an I-frame.
    * - ``V4L2_FWHT_FL_COMPONENTS_NUM_MSK``
      - 0x00070000
      - The number of color components minus one.
    * - ``V4L2_FWHT_FL_PIXENC_MSK``
      - 0x00180000
      - The mask for the pixel encoding.
    * - ``V4L2_FWHT_FL_PIXENC_YUV``
      - 0x00080000
      - Set if the pixel encoding is YUV.
    * - ``V4L2_FWHT_FL_PIXENC_RGB``
      - 0x00100000
      - Set if the pixel encoding is RGB.
    * - ``V4L2_FWHT_FL_PIXENC_HSV``
      - 0x00180000
      - Set if the pixel encoding is HSV.

.. raw:: latex

    \normalsize

.. _v4l2-codec-stateless-vp8:

``V4L2_CID_STATELESS_VP8_FRAME (struct)``
    Specifies the frame parameters for the associated VP8 parsed frame data.
    This includes the necessary parameters for
    configuring a stateless hardware decoding pipeline for VP8.
    The bitstream parameters are defined according to :ref:`vp8`.

.. c:type:: v4l2_ctrl_vp8_frame

.. raw:: latex

    \small

.. tabularcolumns:: |p{7.0cm}|p{4.6cm}|p{5.7cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_vp8_frame
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct :c:type:`v4l2_vp8_segment`
      - ``segment``
      - Structure with segment-based adjustments metadata.
    * - struct :c:type:`v4l2_vp8_loop_filter`
      - ``lf``
      - Structure with loop filter level adjustments metadata.
    * - struct :c:type:`v4l2_vp8_quantization`
      - ``quant``
      - Structure with VP8 dequantization indices metadata.
    * - struct :c:type:`v4l2_vp8_entropy`
      - ``entropy``
      - Structure with VP8 entropy coder probabilities metadata.
    * - struct :c:type:`v4l2_vp8_entropy_coder_state`
      - ``coder_state``
      - Structure with VP8 entropy coder state.
    * - __u16
      - ``width``
      - The width of the frame. Must be set for all frames.
    * - __u16
      - ``height``
      - The height of the frame. Must be set for all frames.
    * - __u8
      - ``horizontal_scale``
      - Horizontal scaling factor.
    * - __u8
      - ``vertical_scaling factor``
      - Vertical scale.
    * - __u8
      - ``version``
      - Bitstream version.
    * - __u8
      - ``prob_skip_false``
      - Indicates the probability that the macroblock is not skipped.
    * - __u8
      - ``prob_intra``
      - Indicates the probability that a macroblock is intra-predicted.
    * - __u8
      - ``prob_last``
      - Indicates the probability that the last reference frame is used
        for inter-prediction
    * - __u8
      - ``prob_gf``
      - Indicates the probability that the golden reference frame is used
        for inter-prediction
    * - __u8
      - ``num_dct_parts``
      - Number of DCT coefficients partitions. Must be one of: 1, 2, 4, or 8.
    * - __u32
      - ``first_part_size``
      - Size of the first partition, i.e. the control partition.
    * - __u32
      - ``first_part_header_bits``
      - Size in bits of the first partition header portion.
    * - __u32
      - ``dct_part_sizes[8]``
      - DCT coefficients sizes.
    * - __u64
      - ``last_frame_ts``
      - Timestamp for the V4L2 capture buffer to use as last reference frame, used
        with inter-coded frames. The timestamp refers to the ``timestamp`` field in
	struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
	function to convert the struct :c:type:`timeval` in struct
	:c:type:`v4l2_buffer` to a __u64.
    * - __u64
      - ``golden_frame_ts``
      - Timestamp for the V4L2 capture buffer to use as last reference frame, used
        with inter-coded frames. The timestamp refers to the ``timestamp`` field in
	struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
	function to convert the struct :c:type:`timeval` in struct
	:c:type:`v4l2_buffer` to a __u64.
    * - __u64
      - ``alt_frame_ts``
      - Timestamp for the V4L2 capture buffer to use as alternate reference frame, used
        with inter-coded frames. The timestamp refers to the ``timestamp`` field in
	struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
	function to convert the struct :c:type:`timeval` in struct
	:c:type:`v4l2_buffer` to a __u64.
    * - __u64
      - ``flags``
      - See :ref:`Frame Flags <vp8_frame_flags>`

.. raw:: latex

    \normalsize

.. _vp8_frame_flags:

``Frame Flags``

.. tabularcolumns:: |p{9.8cm}|p{0.8cm}|p{6.7cm}|

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP8_FRAME_FLAG_KEY_FRAME``
      - 0x01
      - Indicates if the frame is a key frame.
    * - ``V4L2_VP8_FRAME_FLAG_EXPERIMENTAL``
      - 0x02
      - Experimental bitstream.
    * - ``V4L2_VP8_FRAME_FLAG_SHOW_FRAME``
      - 0x04
      - Show frame flag, indicates if the frame is for display.
    * - ``V4L2_VP8_FRAME_FLAG_MB_NO_SKIP_COEFF``
      - 0x08
      - Enable/disable skipping of macroblocks with no non-zero coefficients.
    * - ``V4L2_VP8_FRAME_FLAG_SIGN_BIAS_GOLDEN``
      - 0x10
      - Sign of motion vectors when the golden frame is referenced.
    * - ``V4L2_VP8_FRAME_FLAG_SIGN_BIAS_ALT``
      - 0x20
      - Sign of motion vectors when the alt frame is referenced.

.. c:type:: v4l2_vp8_entropy_coder_state

.. cssclass:: longtable

.. tabularcolumns:: |p{1.0cm}|p{2.0cm}|p{14.3cm}|

.. flat-table:: struct v4l2_vp8_entropy_coder_state
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``range``
      - coder state value for "Range"
    * - __u8
      - ``value``
      - coder state value for "Value"-
    * - __u8
      - ``bit_count``
      - number of bits left.
    * - __u8
      - ``padding``
      - Applications and drivers must set this to zero.

.. c:type:: v4l2_vp8_segment

.. cssclass:: longtable

.. tabularcolumns:: |p{1.2cm}|p{4.0cm}|p{12.1cm}|

.. flat-table:: struct v4l2_vp8_segment
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s8
      - ``quant_update[4]``
      - Signed quantizer value update.
    * - __s8
      - ``lf_update[4]``
      - Signed loop filter level value update.
    * - __u8
      - ``segment_probs[3]``
      - Segment probabilities.
    * - __u8
      - ``padding``
      - Applications and drivers must set this to zero.
    * - __u32
      - ``flags``
      - See :ref:`Segment Flags <vp8_segment_flags>`

.. _vp8_segment_flags:

``Segment Flags``

.. raw:: latex

    \small

.. tabularcolumns:: |p{10cm}|p{1.0cm}|p{6.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP8_SEGMENT_FLAG_ENABLED``
      - 0x01
      - Enable/disable segment-based adjustments.
    * - ``V4L2_VP8_SEGMENT_FLAG_UPDATE_MAP``
      - 0x02
      - Indicates if the macroblock segmentation map is updated in this frame.
    * - ``V4L2_VP8_SEGMENT_FLAG_UPDATE_FEATURE_DATA``
      - 0x04
      - Indicates if the segment feature data is updated in this frame.
    * - ``V4L2_VP8_SEGMENT_FLAG_DELTA_VALUE_MODE``
      - 0x08
      - If is set, the segment feature data mode is delta-value.
        If cleared, it's absolute-value.

.. raw:: latex

    \normalsize

.. c:type:: v4l2_vp8_loop_filter

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{3.9cm}|p{11.9cm}|

.. flat-table:: struct v4l2_vp8_loop_filter
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s8
      - ``ref_frm_delta[4]``
      - Reference adjustment (signed) delta value.
    * - __s8
      - ``mb_mode_delta[4]``
      - Macroblock prediction mode adjustment (signed) delta value.
    * - __u8
      - ``sharpness_level``
      - Sharpness level
    * - __u8
      - ``level``
      - Filter level
    * - __u16
      - ``padding``
      - Applications and drivers must set this to zero.
    * - __u32
      - ``flags``
      - See :ref:`Loop Filter Flags <vp8_loop_filter_flags>`

.. _vp8_loop_filter_flags:

``Loop Filter Flags``

.. tabularcolumns:: |p{7.0cm}|p{1.2cm}|p{9.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP8_LF_ADJ_ENABLE``
      - 0x01
      - Enable/disable macroblock-level loop filter adjustment.
    * - ``V4L2_VP8_LF_DELTA_UPDATE``
      - 0x02
      - Indicates if the delta values used in an adjustment are updated.
    * - ``V4L2_VP8_LF_FILTER_TYPE_SIMPLE``
      - 0x04
      - If set, indicates the filter type is simple.
        If cleared, the filter type is normal.

.. c:type:: v4l2_vp8_quantization

.. tabularcolumns:: |p{1.5cm}|p{3.5cm}|p{12.3cm}|

.. flat-table:: struct v4l2_vp8_quantization
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``y_ac_qi``
      - Luma AC coefficient table index.
    * - __s8
      - ``y_dc_delta``
      - Luma DC delta vaue.
    * - __s8
      - ``y2_dc_delta``
      - Y2 block DC delta value.
    * - __s8
      - ``y2_ac_delta``
      - Y2 block AC delta value.
    * - __s8
      - ``uv_dc_delta``
      - Chroma DC delta value.
    * - __s8
      - ``uv_ac_delta``
      - Chroma AC delta value.
    * - __u16
      - ``padding``
      - Applications and drivers must set this to zero.

.. c:type:: v4l2_vp8_entropy

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_vp8_entropy
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``coeff_probs[4][8][3][11]``
      - Coefficient update probabilities.
    * - __u8
      - ``y_mode_probs[4]``
      - Luma mode update probabilities.
    * - __u8
      - ``uv_mode_probs[3]``
      - Chroma mode update probabilities.
    * - __u8
      - ``mv_probs[2][19]``
      - MV decoding update probabilities.
    * - __u8
      - ``padding[3]``
      - Applications and drivers must set this to zero.
