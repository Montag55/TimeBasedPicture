class PaintMask {
    constructor(canvas) {
        this.canvas = canvas;
        this.context = canvas.getContext('2d');
        this.dom = document.createElement('div');
        this.dom.classList.add('paintMask');

        this.addButton = document.createElement('button');
        this.addButton.innerHTML = '+';
        this.dom.appendChild(this.addButton);

        this.showMask = document.createElement('input');
        this.showMask.type = 'checkbox';
        this.dom.appendChild(this.showMask);
        this.showMask.addEventListener('change', (function () {
            this.canvas.style.display = (this.showMask.checked) ? 'block' : 'none';
        }).bind(this));

        this.brushSize = document.createElement('input');
        this.brushSize.type = 'range';
        this.brushSize.min = 1;
        this.brushSize.max = 1000;
        this.brushSize.value = 10;
        this.dom.appendChild(this.brushSize);
        this.brushSize.style.marginBottom = '1.5vh';

        this.colorContainer = document.createElement('div');
        this.dom.appendChild(this.colorContainer);

        this.active = null;
        this.activeColor = 'rgba(0, 0, 0, 0)';
        this.colors = [];

        this.canvas.addEventListener('mousedown', (function (e) {
            this.left = e.button === 0;
            this.right = e.button === 2;
        }).bind(this));

        this.canvas.addEventListener('mousemove', (function (e) {
            if (this.showMask.checked === false) {
                return;
            }           
            if (this.left) {
                this.change = true;
                this.context.globalCompositeOperation = 'source-over';
                this.context.fillStyle = this.activeColor;
                this.context.beginPath();
                let p = this.getScale({x: e.pageX, y: e.pageY});
                // this.context.arc(p.x, p.y, this.brushSize.value, 0, 2 * Math.PI);
                this.aliasedCircle(this.context, p.x, p.y, this.brushSize.value);
                this.context.fill();
            } else if (this.right) {
                this.change = true;
                this.context.globalCompositeOperation = 'destination-out';
                this.context.beginPath();
                let p = this.getScale({x: e.pageX, y: e.pageY});
                // this.context.arc(p.x, p.y, this.brushSize.value, 0, Math.PI * 2);
                this.aliasedCircle(this.context, p.x, p.y, this.brushSize.value);

                this.context.fill();
            }
        }).bind(this));

        window.addEventListener('mouseup', (function (e) {
            this.left = false;
            this.right = false;
            if (this.change === true) {
                if (typeof (Interpretations.getActive()) !== 'number') {
                    return;
                }
                let dataUrl = this.canvas.toDataURL('image/png');
                let base64Data = dataUrl.replace(/^data:image\/png;base64,/, "");
                fs.writeFile(`maskPaintInterpretation${Interpretations.getActive()}.png`, base64Data, 'base64', function (err) {
                    console.log(err);
                });
                Interpretations.update();
            }
            this.change = false;
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
    
    aliasedCircle(ctx, xc, yc, r) {  // NOTE: for fill only!
        xc = parseInt(xc);
        yc = parseInt(yc);
        r = parseInt(r);
        var x = r, y = 0, cd = 0;
      
        // middle line
        ctx.rect(xc - x, yc, r<<1, 1);
      
        while (x > y) {
          cd -= (--x) - (++y);
          if (cd < 0) cd += x++;
          ctx.rect(xc - y, yc - x, y<<1, 1);    // upper 1/4
          ctx.rect(xc - x, yc - y, x<<1, 1);    // upper 2/4
          ctx.rect(xc - x, yc + y, x<<1, 1);    // lower 3/4
          ctx.rect(xc - y, yc + x, y<<1, 1);    // lower 4/4
        }
      }

    getScale(pos) {
        let rect = this.canvas.getBoundingClientRect();
        let scaleX = this.canvas.width / rect.width;
        let scaleY = this.canvas.height / rect.height;
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
        if (typeof (data) !== 'ImageData') {
            data = this.context.createImageData(this.canvas.width,this.canvas.height);
        }
        this.context.putImageData(data, 0, 0);
        let dataUrl = this.canvas.toDataURL('image/png');
        let base64Data = dataUrl.replace(/^data:image\/png;base64,/, "");
        fs.writeFile(`maskPaintInterpretation${Interpretations.getActive()}.png`, base64Data, 'base64', function (err) {
            console.log(err);
        });
    }

    getImageData() {
        return this.context.getImageData(0, 0, this.canvas.width, this.canvas.height);
    }

    setValues(v) {
        this.colors = [];
        this.colorContainer.innerHTML = '';
        console.log(this.colorContainer.innerHTML);
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
            this.update();
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