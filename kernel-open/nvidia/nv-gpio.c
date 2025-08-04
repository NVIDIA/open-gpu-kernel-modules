/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 - 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"

#include "os_gpio.h"

#define NV_GPIOF_DIR_IN    (1 << 0)

NV_STATUS NV_API_CALL nv_gpio_get_pin_state
(
    nv_state_t *nv,
    NvU32 pinNum,
    NvU32 *pinValue
)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    int ret;

    ret = gpio_get_value(pinNum);
    if (ret < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: %s: failed with err: %d\n",
                  __func__, ret);
        return NV_ERR_GENERIC;
    }

    *pinValue = ret;

    return NV_OK;
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: platform device support not present\n");
    return NV_ERR_GENERIC;
#endif
}

void NV_API_CALL nv_gpio_set_pin_state
(
    nv_state_t *nv,
    NvU32 pinNum,
    NvU32 pinValue
)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    gpio_set_value(pinNum, pinValue);
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: platform device support not present\n");
#endif
}

NV_STATUS NV_API_CALL nv_gpio_set_pin_direction
(
    nv_state_t *nv,
    NvU32 pinNum,
    NvU32 direction
)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    int ret;

    if (direction)
    {
        ret = gpio_direction_input(pinNum);
    }
    else
    {
        ret = gpio_direction_output(pinNum, 0);
    }

    if (ret)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: %s: failed with err: %d\n",
                  __func__, ret);
        return NV_ERR_GENERIC;
    }

    return NV_OK;
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: platform device support not present\n");
    return NV_ERR_GENERIC;
#endif
}

NV_STATUS NV_API_CALL nv_gpio_get_pin_direction
(
    nv_state_t *nv,
    NvU32 pinNum,
    NvU32 *direction
)
{
/*!
 * TODO: Commenting out until gpio_get_direction wrapper
 * support is added in kernel.
 */
#if 0
    int ret;

    ret = nv_gpio_get_direction(pinNum);
    if (ret)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: %s: failed with err: %d\n",
                  __func__, ret);
        return NV_ERR_GENERIC;
    }
    *direction = ret;
#endif

    return NV_OK;
}

NV_STATUS NV_API_CALL nv_gpio_get_pin_number
(
    nv_state_t *nv,
    NvU32 function,
    NvU32 *pinNum
)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int rc;

    /*!
     * @brief Mapping array of OS GPIO function ID to OS function name,
     * this name is used to get GPIO number from Device Tree.
     */
    static const char *osMapGpioFunc[] = {
        [NV_OS_GPIO_FUNC_HOTPLUG_A] = "os_gpio_hotplug_a",
        [NV_OS_GPIO_FUNC_HOTPLUG_B] = "os_gpio_hotplug_b",
        [NV_OS_GPIO_FUNC_HOTPLUG_C] = "os_gpio_hotplug_c",
        [NV_OS_GPIO_FUNC_HOTPLUG_D] = "os_gpio_hotplug_d",
    };

    rc = of_get_named_gpio(nvl->dev->of_node, osMapGpioFunc[function], 0);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: of_get_name_gpio failed for gpio - %s, rc - %d\n",
                  osMapGpioFunc[function], rc);
        return NV_ERR_GENERIC;
    }
    *pinNum = rc;

    rc = devm_gpio_request_one(nvl->dev, *pinNum, NV_GPIOF_DIR_IN,
                               osMapGpioFunc[function]);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: request gpio failed for gpio - %s, rc - %d\n",
                  osMapGpioFunc[function], rc);
        return NV_ERR_GENERIC;
    }

    return NV_OK;
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: platform device support not present\n");
    return NV_ERR_GENERIC;
#endif
}

NvBool NV_API_CALL nv_gpio_get_pin_interrupt_status
(
    nv_state_t *nv,
    NvU32 pinNum,
    NvU32 direction
)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    NvU32 irqGpioPin;
    NvU32 pinValue;

    if (nv_get_current_irq_type(nv) != NV_SOC_IRQ_GPIO_TYPE)
    {
        return NV_FALSE;
    }

    nv_get_current_irq_priv_data(nv, &irqGpioPin);
    if (pinNum != irqGpioPin)
    {
        return NV_FALSE;
    }

    pinValue = gpio_get_value(pinNum);
    if (pinValue != direction)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: platform device support not present\n");
    return NV_FALSE;
#endif
}

NV_STATUS NV_API_CALL nv_gpio_set_pin_interrupt
(
    nv_state_t * nv,
    NvU32 pinNum,
    NvU32 trigger_level
)
{
#if NV_SUPPORTS_PLATFORM_DEVICE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int rc;
    int irq_num;

    irq_num = gpio_to_irq(pinNum);

    /*
     * Ignore setting interrupt for falling trigger for hotplug gpio pin
     * as hotplug sequence calls this function twice to set the level
     * (rising/falling) of interrupt for same gpio pin. Linux interrupt
     * registration allows only once to register the interrupt with required
     * trigger levels. So to avoid re-registration, skip registering for
     * falling trigger level but when this function called with rising trigger
     * then itself register for both rising/falling triggers.
     */
    if (trigger_level == 0)
    {
        return NV_OK;
    }

    rc = nv_request_soc_irq(nvl, irq_num, NV_SOC_IRQ_GPIO_TYPE,
                            (IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |
                            IRQF_ONESHOT), pinNum,
                            "hdmi-hotplug");
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: IRQ registration failed for gpio - %d, rc - %d\n",
                  pinNum, rc);
        return NV_ERR_GENERIC;
    }

    /* Disable the irq after registration as RM init sequence re-enables it */
    disable_irq_nosync(irq_num);

    return NV_OK;
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: platform device support not present\n");
    return NV_ERR_GENERIC;
#endif
}
