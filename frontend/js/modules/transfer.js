class TransferFunction {
  constructor(width, height) {
    this.width = width;
    this.height = height;
    this.canvas = document.createElement('canvas');
    this.canvas.width = this.width;
    this.canvas.height = this.height;
    this.ctx = this.canvas.getContext('2d');
    this.points = [new Point(0, 0.9), new Point(1, 0.9)];
    this.dragged = null;
    this.pointID = null;
    this.draw();

    window.addEventListener("mousedown", (function(e){
      if(e.button === 0) {
        this.points.forEach((function(entry, idx){
          if(entry.isInside({x: this.transformX(e.pageX)/width, y: this.transformY(e.pageY)/height}, this.width, this.height)){
            this.dragged = entry;
            this.pointID = idx;
            return;
          }
        }).bind(this));
      }
    }).bind(this));

    window.addEventListener("mouseup", (function(e){
      if(e.button === 0) {
        if(this.dragged != null){
          this.dragged = null;
        }
        else if(this.transformX(e.pageX) < this.width && this.transformY(e.pageY) < this.height && e.target === this.canvas){
          this.points.push(new Point(this.transformX(e.pageX)/this.width, this.transformY(e.pageY)/this.height));
          this.points.sort(function(a, b){
            return a.x - b.x;
          });
        }
      } else if(e.button === 2){
        for(let i = 1, len = this.points.length - 1; i < len; i++){
          if(this.points[i].isInside({x: this.transformX(e.pageX)/this.width, y: this.transformY(e.pageY)/this.height}, this.width, this.height)){
            this.points.splice(i, 1);
            break;
          }
        }
      }
      this.draw();
    }).bind(this));

    window.addEventListener("mousemove", (function(e){
      if(this.dragged != null){
        if(this.pointID !== 0 && this.pointID !== this.points.length - 1){
          this.dragged.x = Math.min(Math.max(this.transformX(e.pageX)/this.width, 0), 1);
        }
        this.dragged.y = Math.min(Math.max(this.transformY(e.pageY)/this.height, 0), 1);

        this.points.sort(function(a, b){
          return a.x - b.x;
        });
        this.draw();
      }
    }).bind(this));

    window.addEventListener('contextmenu', function(e){e.preventDefault();});
  }

  setValues(v) {
    this.points = [];
    v.forEach((function (entry, index) {
      this.points.push(new Point(1 / (v.length - 1) * index, entry));
    }).bind(this));
  }

  draw() {
    this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);

    for(let i = 0, len = this.points.length - 1; i < len; i++){
      this.drawLine(this.points[i], this.points[i+1]);
    }
    for(let i = 0, len = this.points.length; i < len; i++){
      this.drawPoint(this.points[i]);
    }
  }

  drawPoint(p) {
    this.ctx.beginPath();
    this.ctx.arc(p.x*this.width, p.y*this.height, p.radius, 0, 2 * Math.PI);
    this.ctx.lineWidth = 1;
    this.ctx.strokeStyle = "#1DE9B6";
    this.ctx.stroke();
    this.ctx.fillStyle = "#00BFA5";
    this.ctx.fill();
  }

  drawLine(p1, p2) {
    this.ctx.beginPath();
    this.ctx.moveTo(p1.x*this.width, p1.y*this.height);
    this.ctx.lineTo(p2.x*this.width, p2.y*this.height);
    this.ctx.lineWidth = 2;
    this.ctx.strokeStyle = "#64FFDA";
    this.ctx.stroke();
  }

  transformX(x){
    let rect = this.canvas.getBoundingClientRect();
    let scaleX = this.canvas.width/rect.width;
    return (x - rect.left) * scaleX;
  }

  transformY(y){
    let rect = this.canvas.getBoundingClientRect();
    let scaleY = this.canvas.height/rect.height;
    return (y - rect.top) * scaleY;
  }

  getValues() {
    let arr = [];
    this.points.forEach(function (p) {
      arr.push(p.y);
    });
    return arr;
  }
}

class Point{
  constructor(x, y, r) {
    this.x = x || 0;
    this.y = y || 0;
    this.radius = r || 4;
  }

  isInside(p, width, height) {
    return Math.pow(p.x*width - this.x*height, 2) + Math.pow(p.y*height - this.y*height, 2) < Math.pow(this.radius, 2);
  }
}