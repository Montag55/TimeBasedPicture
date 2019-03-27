class PointPicker {
    constructor(imgs) {

        this.points = [];
        this.dom = document.createElement('div');
        this.dom.classList.add('pointPicker');

        let pick = document.createElement('button');
        pick.innerText = '+';
        this.dom.appendChild(pick);
        this.picking = false;

        let togglePointView = document.createElement('input');
        togglePointView.type = 'checkbox';
        togglePointView.checked = true;
        this.dom.appendChild(togglePointView);
        togglePointView.style.width = '50%';

        let pointCanvas = document.createElement('canvas');
        let pointCtx = pointCanvas.getContext('2d');
        document.querySelector('.player .center').appendChild(pointCanvas);
        pointCanvas.style.position = 'absolute';
        pointCanvas.style.top = '0';
        pointCanvas.style.left = '0';
        pointCanvas.style.width = '100%';
        pointCanvas.width = 1920;
        pointCanvas.height = 1080;
        pointCanvas.style.background = 'transparent';
        pointCanvas.style.display = 'block';
        pointCanvas.style.pointerEvents = 'none';
        pointCanvas.style.opacity = 0.75;
        this.pointCanvas = pointCanvas;
        this.pointCtx = pointCtx;


        this.dom.addEventListener('contextmenu', function (e) {e.preventDefault();});

        togglePointView.addEventListener('change', function () {
            pointCanvas.style.display = (togglePointView.checked) ? 'block' : 'none';
        });

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
                    pointDom.innerText = `(${x.toString().substring(0, 5)}, ${y.toString().substring(0, 5)})`;
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

    drawPoints() {
        this.pointCtx.clearRect(0, 0, this.pointCanvas.width, this.pointCanvas.height);
        this.points.forEach((function (p) {
            let img = document.getElementById('bla');

            this.pointCtx.beginPath();
            this.pointCtx.fillStyle = 'red';
            this.pointCtx.font = "lighter 90px Arial";
            this.pointCtx.textBaseline = "middle"; 
            this.pointCtx.textAlign = "center";
            this.pointCtx.fillText("+", p.x * this.pointCanvas.width, p.y * this.pointCanvas.height);
        }).bind(this));
    }

    onChange(f) {
        this.update = function () {
            this.drawPoints();
            f();
        };
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
