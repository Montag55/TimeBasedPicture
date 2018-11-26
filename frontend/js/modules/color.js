class ColorPicker {
    constructor(imgs) {
        this.colors = [];
        this.dom = document.createElement('div');
        this.dom.classList.add('colorPicker');
        let pick = document.createElement('button');
        pick.innerText = '+';
        this.dom.appendChild(pick);
        this.picking = false;

        this.dom.addEventListener('contextmenu', function (e) {e.preventDefault();})

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
                    let canvas = document.createElement('canvas');
                    canvas.width = img.width;
                    canvas.height = img.height;
                    console.log(img.width);

                    let context = canvas.getContext('2d');
                    context.drawImage(img, 0, 0, img.width, img.height);
                   // document.body.appendChild(canvas);

                    let rect = img.getBoundingClientRect(); // abs. size of element
                    let scaleX = img.width / rect.width;   // relationship bitmap vs. element for X
                    let scaleY = img.height / rect.height;  // relationship bitmap vs. element for Y
              
                    let x = (e.clientX - rect.left) * scaleX;   // scale mouse coordinates after they have
                    let y = (e.clientY - rect.top) * scaleY;   // been adjusted to be relative to element

                    let colorData = context.getImageData(x, y, 1, 1).data.slice(0, 3);

                    this.colors.push(colorData);
                    console.log(this.colors);

                    let color = `rgb(${colorData[0]}, ${colorData[1]}, ${colorData[2]})`;
                    let colorDom = document.createElement('div');
                    colorDom.classList.add('color');
                    colorDom.style.backgroundColor = color;
                    this.dom.appendChild(colorDom);
                    colorDom.addEventListener('mouseup', (function (e) {
                        if (e.button === 2) {
                            this.dom.removeChild(colorDom);
                            let index  = this.colors.indexOf(colorData);
                            console.log(index);
                            if (index > -1) {
                                this.colors.splice(index, 1);
                                console.log(this.colors);
                            }
                            this.update();
                        }
                    }).bind(this));
                    this.update();
                }
                this.picking = false;
            }).bind(this))
        }).bind(this));
    }

    onChange(f) {
        this.update = f;
    }

    getValues() {
        let arr = [];
        this.colors.forEach(function (entry) {
            arr = arr.concat(Array.from(entry).reverse());
        });
        return arr;
    }

    setValues(v) {
        this.colors = [];
        for (let i = 0, len = v.length; i < len; i += 3) {
            let colorData = [v[i], v[i + 1], v[i + 2]];
            this.colors.push(colorData);

            let color = `rgb(${colorData[0]}, ${colorData[1]}, ${colorData[2]})`;
            let colorDom = document.createElement('div');
            colorDom.classList.add('color');
            colorDom.style.backgroundColor = color;
            this.dom.appendChild(colorDom);
            colorDom.addEventListener('mouseup', (function (e) {
                if (e.button === 2) {
                    this.dom.removeChild(colorDom);
                    let index  = this.colors.indexOf(colorData);
                    console.log(index);
                    if (index > -1) {
                        this.colors.splice(index, 1);
                        console.log(this.colors);
                    }
                }
            }).bind(this));
        }
    }
}