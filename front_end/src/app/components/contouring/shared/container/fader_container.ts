import { BaseContainer } from "./base_container";
import { Fader } from "../overlay/fader";
import { Point } from "../tools/point";

export class FaderContainer extends BaseContainer {
	fader: Fader;
	prePos: Point;

	constructor(stage) {
		super(stage, 'nudge');
		this.fader = new Fader(stage);
		this.disable = false;
	}

	update() {
		this.fader.update(this.fader.center);
	}

	handleKeyDown(e) {
		console.log(e.keyCode)
		if (e.keyCode == 187) { // 189 -
			++this.fader.radius;
		} else if (e.keyCode == 189) { // 187 +
			--this.fader.radius;
		}
		this.fader.update(this.fader.center);
	}

	handleMouseDown(evt) {
		console.log('[line]handle MouseDown')
		this.prePos = new Point(evt.offsetX, evt.offsetY)
		this.fader.center = this.prePos;
		this.fader.update(this.fader.center)

        super.handleMouseDown(evt)
		this.isMousedown = true;
		this.currentMouseButton = evt.button;
        if (evt.target.type != 'line' && evt.target.type != 'controlpoint' && evt.target.type != 'text') {
            this.updateCp(0, evt.offsetX, evt.offsetY)
        }
    }
    handleMouseMove(e) {
		this.fader.center = new Point(e.offsetX, e.offsetY);
		this.fader.update(this.fader.center)

        if (this.isMousedown) {
			let curPos = new Point(e.offsetX, e.offsetY)
			if (this.currentMouseButton == 1) {
				if (curPos.y - this.prePos.y < 0) {
					if (this.fader.radius < 80)
						this.fader.radius += 1;
				} else {
					if (this.fader.radius > 5)
						this.fader.radius -= 1;
				}
			}
			this.prePos = curPos;
			this.fader.update(this.fader.center)
			
            this.update();
        }
    }
    handleMouseUp(e) {
        console.log('[line]handle MouseUp')
		this.isMousedown = false;
		this.currentMouseButton = -1;
        this.isPaint = false;
    }

    handlePressMove(e) {
        this.fader.center = new Point(e.offsetX, e.offsetY);
		this.fader.update(this.fader.center)
        this.update();
	}

	handleMouseLeave(e) {
	}
}
