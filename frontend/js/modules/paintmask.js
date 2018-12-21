class PaintMask {
    constructor(canvas) {
        this.canvas = canvas;
        this.context = canvas.getContext('2d');
        this.dom = document.createElement('div');
        this.dom.classList.add('paintMask');

        this.addButton = document.createElement('button');
        this.addButton.innerHTML = '+';
        this.dom.appendChild(this.addButton);

        this.brushSize = document.createElement('input');
        this.brushSize.type = 'slider';
        this.brushSize.min = 1;
        this.brushSize.max = 1000;
        this.brushSize.value = 10;

        this.colorContainer = document.createElement('div');
        this.dom.appendChild(this.colorContainer);

        this.active = null;
        this.activeColor = 'rgba(0, 0, 0, 0)';
        this.colors = [];

        this.canvas.addEventListener('click', (function () {
            this.context.fillStyle = this.activeColor;
            this.context.fillRect(10, 10, 50, 50);
        }).bind(this));

        this.addButton.addEventListener('click', (function () {
            let color = new Color(Math.floor(Math.random() * 255), Math.floor(Math.random() * 255), Math.floor(Math.random() * 255), 0, 1);
            this.colors.push(color);

            let colorDom = document.createElement('div');
            colorDom.classList.add('color');

            let prev = document.createElement('div');
            prev.classList.add('prev');
            //Colors as RGB 0 - 255????!??!?
            prev.style.backgroundColor = `rgb(${color.r}, ${color.g}, ${color.b})`;
            colorDom.appendChild(prev);

            let start = document.createElement('input');
            start.type = "number";
            start.min = 0;
            start.value = color.start;
            start.addEventListener('change', (function () {
                color.start = start.value;
                this.update();
            }).bind(this));
            colorDom.appendChild(start);

            let end = document.createElement('input');
            end.type = "number";
            end.min = 1;
            end.value = color.end;
            end.addEventListener('change', (function () {
                color.end = end.value;
                this.update();
            }).bind(this));
            colorDom.appendChild(end);

            colorDom.addEventListener('click', (function () {
                if (this.active) {
                    this.active.classList.remove('active');
                }
                this.active = colorDom;
                this.active.classList.add('active');
                this.activeColor = `rgb(${color.r}, ${color.g}, ${color.b})`;
            }).bind(this));

            this.colorContainer.appendChild(colorDom);
            this.update();
        }).bind(this));
    }

    getScale(pos) {
        let rect = this.canvas.getBoundingClientRect();
        let scaleX = canvas.width / rect.width;
        let scaleY = canvas.height / rect.height;
        let x = scaleX * (pos.x - rect.left);
        let y = scaleY * (pos.y - rect.top);
        return {
            x: x,
            y: y
        };
    }

    getValues() {
        let arr = [];
        this.colors.forEach(function (c) {
            arr = arr.concat([c.r, c.g, c.b, c.start, c.end]);
        });
        return arr;
    }

    onChange(f) {
        this.update = f;
    }

    setImageData(data) {
        this.context.putImageData(data, 0, 0);
    }

    getImageData() {
        return this.context.getImageData(0, 0, this.canvas.width, this.canvas.height);
    }

    setValues(v) {
        this.colors = [];
        this.colorContainer.innerHTML = '';
        for (let i = 0, len = v.length; i < len; i += 5) {
            let color = new Color(v[i], v[i + 1], v[i + 2], v[i + 3], v[i + 4]);
            this.colors.push(color);

            let colorDom = document.createElement('div');
            colorDom.classList.add('color');

            let prev = document.createElement('div');
            prev.classList.add('prev');
            //Colors as RGB 0 - 255????!??!?
            prev.style.backgroundColor = `rgb(${color.r}, ${color.g}, ${color.b})`;
            colorDom.appendChild(prev);

            let start = document.createElement('input');
            start.type = "number";
            start.min = 0;
            start.value = color.start;
            start.addEventListener('change', (function () {
                color.start = start.value;
                this.update();
            }).bind(this));
            colorDom.appendChild(start);

            let end = document.createElement('input');
            end.type = "number";
            end.min = 0;
            end.value = color.end;
            end.addEventListener('change', (function () {
                color.end = end.value;
                this.update();
            }).bind(this));
            colorDom.appendChild(end);

            colorDom.addEventListener('click', (function () {
                if (this.active) {
                    this.active.classList.remove('active');
                }
                this.active = colorDom;
                this.active.classList.add('active');
                this.activeColor = `rgb(${color.r}, ${color.g}, ${color.b})`;
            }).bind(this));

            this.colorContainer.appendChild(colorDom);
        }
    }
}

class Color {
    constructor(r, g, b, start, end) {
        this.r = r;
        this.g = g;
        this.b = b;
        this.start = start;
        this.end = end;
    }
}