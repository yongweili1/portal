import { BaseAction } from './base';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { ActionTypeEnum } from '../../../../shared/models/enums';


export class WWWL extends BaseAction {
    ww_factor = 1;
    wl_factor = 0;
    preX = 0;
    preY = 0;

    handleMouseDown(e) {
        super.handleMouseDown(e);
        this.ww_factor = 1;
        this.wl_factor = 0;
        this.preX = e.clientX;
        this.preY = e.clientY;
        console.log('mouse down', this.preX, this.preY);
    }

    handleMouseMove(e) {
        if (!this.isMouseDown) {
            return;
        }
        const curX = e.clientX;
        const curY = e.clientY;
        const shiftX = curX - this.preX;
        const shiftY = curY - this.preY;

        if (shiftY >= 0) {
            this.ww_factor = 1.0 + shiftY * 1.0 / 600;
        } else {
            this.ww_factor = 1.0 / (1.0 - shiftY * 1.0 / 600);
        }
        this.wl_factor = (this.preX - curX) * 1.0 / 1200;
        this.preX = curX;
        this.preY = curY;

        const data = [];
        data[0] = ActionTypeEnum.window;

        // 判断方向
        if (Math.abs(shiftY) >= Math.abs(shiftX)) {
            this.wl_factor = 0;
        } else {
            this.ww_factor = 1;
        }

        // 判断取值范围
        if (this.ww_factor > 1.03) {
            this.ww_factor = 1.03;
        }
        if (this.ww_factor < -1.03) {
            this.ww_factor = -1.03;
        }
        if (this.wl_factor > 0.02) {
            this.wl_factor = 0.02;
        }
        if (this.wl_factor < -0.02) {
            this.wl_factor = -0.02;
        }
        data[1] = [this.tag, this.ww_factor, this.wl_factor];
        EventAggregator.Instance().eventData.publish(data);
    }

    handleMouseUp(e) {
        super.handleMouseUp(e);
    }
}
