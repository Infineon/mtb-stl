#ifndef MTB_STL_POST_CFG_H
#define MTB_STL_POST_CFG_H

#include <zephyr/kernel.h>
#include <zephyr/irq.h>
#include <zephyr/post/post.h>
#include <zephyr/post/post_vendor.h>
#include <zephyr/logging/log.h>
#include <zephyr/devicetree.h>

#include "cy_pdl.h"
#include "mtb_stl_post.h"

/* State saved before POST test */
typedef struct {
        uint32_t period;
        uint32_t compare0;
        uint32_t compare1;
        uint32_t counter_value;
        uint32_t interrupt_mask;
        bool enabled;
        /* Save counter driver's terminal count interrupt state */
        bool counter_driver_tc_enabled;
} infineon_stl_saved_counter_state_t;

extern const cy_stc_tcpwm_counter_config_t ifx_timer_config;
extern const cy_stc_tcpwm_counter_config_t ifx_clk_test_timer_config;
extern const cy_stc_sar_channel_config_t ifx_dut_sar_adc_ch_0_cfg;
extern const cy_stc_sar_config_t ifx_dut_sar_adc_cfg;
extern const cy_stc_scb_uart_config_t ifx_uart_test_cfg;
extern const cy_stc_tcpwm_counter_config_t ifx_timer_counter_config;
extern const cy_stc_tcpwm_pwm_config_t ifx_pwm_config;
extern const cy_stc_tcpwm_pwm_config_t ifx_pwm_gatekill_config;
extern const cy_stc_scb_uart_config_t ifx_uart_master_config;
extern const cy_stc_scb_uart_config_t ifx_uart_slave_cfg;
extern const cy_stc_tcpwm_counter_config_t ifx_timer_uart_master_cfg;

static inline bool infineon_stl_cnt_enabled(TCPWM_Type const *base, uint32_t cntnum)
{
	uint32_t counter_status;
	bool enabled;

	/* Check if counter is enabled/running */
	counter_status = Cy_TCPWM_Counter_GetStatus(base, cntnum);
	enabled = (counter_status & CY_TCPWM_COUNTER_STATUS_COUNTER_RUNNING) != 0;

	return enabled;
}

static inline void infineon_stl_stop_running_cnt(TCPWM_Type *base, uint32_t cntnum)
{
	/* Stop counter if running */
#if defined(CY_IP_M0S8TCPWM)
	uint32_t shiftedValue = (uint32_t)1U << cntnum;
	Cy_TCPWM_TriggerStopOrKill(base, shiftedValue);
#else
	Cy_TCPWM_TriggerStopOrKill_Single(base, cntnum);
#endif
}

static inline void infineon_stl_start_cnt(bool savedcnt_enabled, TCPWM_Type *base, uint32_t cntnum)
{
	if (savedcnt_enabled) {
		/* Stop counter if running */
#if defined(CY_IP_M0S8TCPWM)
		uint32_t shiftedValue = (uint32_t)1U << cntnum;
		Cy_TCPWM_TriggerStart(base, shiftedValue);
#else
		Cy_TCPWM_TriggerStart_Single(base, cntnum);
#endif
	}
}

static inline void infineon_stl_save_current_cnt_state(infineon_stl_saved_counter_state_t *saved_cnt_state,
						TCPWM_Type const *base, uint32_t cntnum)
{
	saved_cnt_state->period = Cy_TCPWM_Counter_GetPeriod(base, cntnum);
	saved_cnt_state->compare0 = Cy_TCPWM_Counter_GetCompare0(base, cntnum);
	saved_cnt_state->compare1 = Cy_TCPWM_Counter_GetCompare1(base, cntnum);
	saved_cnt_state->counter_value = Cy_TCPWM_Counter_GetCounter(base, cntnum);
	saved_cnt_state->interrupt_mask = Cy_TCPWM_GetInterruptMask(base, cntnum);
}

/* Configure counter with POST test parameters */
static inline void infineon_stl_config_post_parameter(const cy_stc_tcpwm_counter_config_t *post_test_cfgs,
						TCPWM_Type *base, uint32_t cntnum)
{
	Cy_TCPWM_Counter_SetPeriod(base, cntnum, post_test_cfgs->period);
	Cy_TCPWM_Counter_SetCompare0(base, cntnum, post_test_cfgs->compare0);
	Cy_TCPWM_Counter_SetCompare1(base, cntnum, post_test_cfgs->compare1);
}

static inline void infineon_stl_restore_cnt_state(infineon_stl_saved_counter_state_t *saved_cnt_state,
						TCPWM_Type *base, uint32_t cntnum)
{
	Cy_TCPWM_Counter_SetPeriod(base, cntnum, saved_cnt_state->period);
	Cy_TCPWM_Counter_SetCompare0(base, cntnum, saved_cnt_state->compare0);
	Cy_TCPWM_Counter_SetCompare1(base, cntnum, saved_cnt_state->compare1);
	Cy_TCPWM_Counter_SetCounter(base, cntnum, saved_cnt_state->counter_value);
	Cy_TCPWM_SetInterruptMask(base, cntnum, saved_cnt_state-> interrupt_mask);
}

static inline void infineon_stl_clk_cfg(cy_en_sysclk_divider_types_t div_type, uint32_t div_ch, uint32_t div_val, en_clk_dst_t periblock_clk)
{
	Cy_SysClk_PeriphDisableDivider(div_type, div_ch);
	Cy_SysClk_PeriphSetDivider(div_type, div_ch, (div_val - 1));
	Cy_SysClk_PeriphEnableDivider(div_type, div_ch);
	Cy_SysClk_PeriphAssignDivider(periblock_clk, div_type, div_ch);
}

static inline GPIO_PRT_Type *ifx_get_port_base(uint32_t port_num)
{
	switch (port_num) {
	case 0:
		return GPIO_PRT0;
	case 1:
		return GPIO_PRT1;
	case 2:
		return GPIO_PRT2;
	case 3:
		return GPIO_PRT3;
	case 4:
		return GPIO_PRT4;
	case 5:
		return GPIO_PRT5;
	case 6:
		return GPIO_PRT6;
	default:
		return NULL;
	}

}

/* Save driver state before test */
static inline int ifx_uart_save_driver_state(const struct device *uart_dev,
						struct uart_driver_state *saved_state)
{
        const struct ifx_cat1_uart_config *config;
        struct ifx_cat1_uart_data *data;
        CySCB_Type *scb_base;
        int ret;

        if (uart_dev == NULL || saved_state == NULL) {
                return -EINVAL;
        }

        if (!device_is_ready(uart_dev)) {
                return -ENODEV;
        }

        config = uart_dev->config;
        data = uart_dev->data;
        scb_base = config->reg_addr;

        /* Save SCB base address and block number */
        saved_state->scb_base = scb_base;
        saved_state->scb_block_num = data->hw_resource.block_num;

        /* Save UART configuration */
        ret = uart_config_get(uart_dev, &saved_state->uart_cfg);
        if (ret != 0) {
                return ret;
        }

        /* Save SCB context and config from driver data */
        saved_state->context = data->context;
        saved_state->scb_config = data->scb_config;

        /* Save clock configuration */
        saved_state->clock_block = data->clock.block;
        saved_state->clock_channel = data->clock.channel;

        /* Try to read current divider - for PSoC4, divider is stored in driver */
        /* We'll save the clock structure and restore it */
        saved_state->clock_enabled = true; /* Assume enabled if driver is initialized */

        /* Save SCB register state */
        saved_state->scb_ctrl = SCB_CTRL(scb_base);
        saved_state->scb_rx_fifo_ctrl = SCB_RX_FIFO_CTRL(scb_base);
        saved_state->scb_tx_fifo_ctrl = SCB_TX_FIFO_CTRL(scb_base);
        saved_state->scb_intr_rx_mask = Cy_SCB_GetRxInterruptMask(scb_base);
        saved_state->scb_intr_tx_mask = Cy_SCB_GetTxInterruptMask(scb_base);

        /* Save interrupt state */
        saved_state->irq_enabled = irq_is_enabled(config->irq_num);
        saved_state->irq_priority = config->irq_priority;

        /* Note: Pin configuration will be saved when we configure for test */
        /* We'll need to get pin info from device tree or pinctrl */

        return 0;
}


/* Restore driver state after test */
static inline int uart_restore_driver_state(const struct device *uart_dev,
						const struct uart_driver_state *saved_state)
{
        const struct ifx_cat1_uart_config *config;
        struct ifx_cat1_uart_data *data;
        CySCB_Type *scb_base;
        int ret;
        unsigned int key;

        if (uart_dev == NULL || saved_state == NULL) {
                return -EINVAL;
        }

        if (!device_is_ready(uart_dev)) {
                return -ENODEV;
        }

        config = uart_dev->config;
        data = uart_dev->data;
        scb_base = config->reg_addr;

        /* Disable interrupts during restore */
        key = irq_lock();

        /* Disable UART first */
        Cy_SCB_UART_Disable(scb_base, NULL);

        /* Restore SCB register state */
        SCB_CTRL(scb_base) = saved_state->scb_ctrl;
        SCB_RX_FIFO_CTRL(scb_base) = saved_state->scb_rx_fifo_ctrl;
        SCB_TX_FIFO_CTRL(scb_base) = saved_state->scb_tx_fifo_ctrl;

        /* Restore SCB context to driver data */
        data->context = saved_state->context;
        data->scb_config = saved_state->scb_config;

        /* Restore UART configuration (this will also restore clock settings) */
        ret = uart_configure(uart_dev, &saved_state->uart_cfg);
        if (ret != 0) {
                irq_unlock(key);
                return ret;
        }

        /* Restore interrupt masks */
        Cy_SCB_SetRxInterruptMask(scb_base, saved_state->scb_intr_rx_mask);
        Cy_SCB_SetTxInterruptMask(scb_base, saved_state->scb_intr_tx_mask);

        /* Re-enable UART */
        Cy_SCB_UART_Enable(scb_base);

        /* Restore interrupt enable state */
        if (saved_state->irq_enabled) {
                irq_enable(config->irq_num);
        } else {
                irq_disable(config->irq_num);
        }

        irq_unlock(key);

        return 0;
}

#endif /* MTB_STL_POST_CFG_H */
