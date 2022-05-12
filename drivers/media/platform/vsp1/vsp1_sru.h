/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * vsp1_sru.h  --  R-Car VSP1 Super Resolution Unit
 *
 * Copyright (C) 2013 Renesas Corporation
 *
 * Contact: Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 */
#ifndef __VSP1_SRU_H__
#define __VSP1_SRU_H__

#include <media/media-entity.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-subdev.h>

#include "vsp1_entity.h"

struct vsp1_device;

#define SRU_PAD_SINK				0
#define SRU_PAD_SOURCE				1

struct vsp1_sru {
	struct vsp1_entity entity;

	struct v4l2_ctrl_handler ctrls;

	unsigned int intensity;
};

static inline struct vsp1_sru *to_sru(struct v4l2_subdev *subdev)
{
	return container_of(subdev, struct vsp1_sru, entity.subdev);
}

struct vsp1_sru *vsp1_sru_create(struct vsp1_device *vsp1);

#endif /* __VSP1_SRU_H__ */
