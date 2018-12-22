class PointPicker {
    constructor(imgs) {
        this.points = [];
        this.dom = document.createElement('div');
        this.dom.classList.add('pointPicker');
        let pick = document.createElement('button');
        pick.innerText = '+';
        this.dom.appendChild(pick);
        this.picking = false;

        this.dom.addEventListener('contextmenu', function (e) {e.preventDefault();});

        pick.addEventListener('click', (function () {
            if (this.picking) {
                this.picking = false;
                pick.classList.remove('active');
            } else {
                pick.classList.add('active');
                this.picking = true;
            }
        }).bind(this));

        imgs.forEach((function (img) {
            img.addEventListener('click', (function (e) {
                if (this.picking) {
                    let rect = img.getBoundingClientRect(); // abs. size of element
                    let scaleX = 1 / rect.width;   // relationship bitmap vs. element for X
                    let scaleY = 1 / rect.height;  // relationship bitmap vs. element for Y

                    let x = (e.clientX - rect.left) * scaleX;   // scale mouse coordinates after they have
                    let y = (e.clientY - rect.top) * scaleY;   // been adjusted to be relative to element
                    //
                    // x = parseInt(x);
                    // y = parseInt(y);

                    let pointData = {x: x, y: y, start: 0, end: 0};
                    this.points.push(pointData);

                    let pointDom = document.createElement('div');
                    pointDom.classList.add('point');
                    pointDom.innerText = `(${x}, ${y})`;
                    let start = document.createElement('input');
                    start.type = 'number';
                    start.value = 0;
                    let end = document.createElement('input');
                    end.type = 'number';
                    end.value = 0;
                    start.addEventListener('change', (function () {
                        pointData.start = start.value;
                        this.update();
                    }).bind(this));
                    end.addEventListener('change', (function () {
                        pointData.end = end.value;
                        this.update();
                    }).bind(this));

                    pointDom.appendChild(start);
                    pointDom.appendChild(end);

                    this.dom.appendChild(pointDom);
                    pointDom.addEventListener('mouseup', (function (e) {
                        if (e.button === 2) {
                            this.dom.removeChild(pointDom);
                            let index = this.points.indexOf(pointData);

                            if (index > -1) {
                                this.points.splice(index, 1);
                            }
                            this.update();
                        }
                    }).bind(this));
                    this.update();
                }
                this.picking = false;
            }).bind(this));
        }).bind(this));
    }

    onChange(f) {
        this.update = f;
    }

    getValues() {
        let arr = [];
        this.points.forEach(function (entry) {
            arr = arr.concat([entry.x, entry.y, entry.start, entry.end]);
        });
        return arr;
    }

    setValues(v) {
        this.points = [];
        for (let i = 0, len = v.length; i < len; i += 4) {
            let pointData = {x: v[i], y: v[i + 1], start: v[i + 2], end: v[i + 3]};
            this.points.push(pointData);

            let pointDom = document.createElement('div');
            pointDom.classList.add('point');
            pointDom.innerText = `(${x}, ${y})`;
            let start = document.createElement('input');
            start.type = 'number';
            start.value = pointData.start;
            let end = document.createElement('input');
            end.type = 'number';
            end.value = pointData.end;
            start.addEventListener('change', (function () {
                pointData.start = start.value;
                this.update();
            }).bind(this));
            end.addEventListener('change', (function () {
                pointData.end = end.value;
                this.update();
            }).bind(this));

            pointDom.appendChild(start);
            pointDom.appendChild(end);

            this.dom.appendChild(pointDom);

            pointDom.addEventListener('mouseup', (function (e) {
                if (e.button === 2) {
                    this.dom.removeChild(pointDom);
                    let index = this.points.indexOf(pointData);

                    if (index > -1) {
                        this.points.splice(index, 1);
                    }
                    this.update();
                }
            }).bind(this));
        }
    }
}
