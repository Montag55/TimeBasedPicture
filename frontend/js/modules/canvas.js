let TFFCanvas = (function(){

  let width, height, canvas, ctx, points, dragged, pointID;

  function init(){

    class Point{
      constructor(x, y, r) {
        this.x = x || 0;
        this.y = y || 0;
        this.radius = r || 6;
      }

      isInside(p) {
        return Math.pow(p.x*width - this.x*height, 2) + Math.pow(p.y*height - this.y*height, 2) < Math.pow(this.radius, 2);
      }
    }

    width = 300;
    height = 300;
    canvas = document.getElementById("TFFCanvas");
    canvas.width = width;
    canvas.height = height;
    ctx = canvas.getContext("2d");
    points = [new Point(0, 0.9), new Point(1, 0.9)];
    dragged = null;
    pointID = null;
    draw();

    window.addEventListener("mousedown", function(e){
      if(e.button === 0) {
        points.forEach(function(entry, idx){
          if(entry.isInside({x: transformX(e.pageX)/width, y: transformY(e.pageY)/height})){
            dragged = entry;
            pointID = idx;
            return;
          }
        });
      }
    });

    window.addEventListener("mouseup", function(e){
      if(e.button === 0) {
        if(dragged != null){
          dragged = null;
        }
        else if(transformX(e.pageX) < width && transformY(e.pageY) < height){
          points.push(new Point(transformX(e.pageX)/width, transformY(e.pageY)/height));
          points.sort(function(a, b){
            return a.x - b.x;
          });
        }
      }
      else if(e.button === 2){
        for(let i = 1, len = points.length - 1; i < len; i++){
          if(points[i].isInside({x: transformX(e.pageX)/width, y: transformY(e.pageY)/height})){
            points.splice(i, 1);
            break;
          }
        }
      }
      draw();
    });

    window.addEventListener("mousemove", function(e){
      if(dragged != null){
        if(pointID !== 0 && pointID !== points.length - 1){
          dragged.x = Math.min(Math.max(transformX(e.pageX)/width, 0), 1);
        }
        dragged.y = Math.min(Math.max(transformY(e.pageY)/height, 0), 1);

        points.sort(function(a, b){
          return a.x - b.x;
        });
        draw();
      }
    });

    window.addEventListener('contextmenu', function(e){e.preventDefault();});
  }

  function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for(let i = 0, len = points.length - 1; i < len; i++){
      drawLine(points[i], points[i+1]);
    }
    for(let i = 0, len = points.length; i < len; i++){
      drawPoint(points[i]);
    }
  }

  function drawPoint(p) {
    ctx.beginPath();
    ctx.arc(p.x*width, p.y*height, p.radius, 0, 2 * Math.PI);
    ctx.lineWidth = 1;
    ctx.strokeStyle = "red";
    ctx.stroke();
    ctx.fillStyle = "orange";
    ctx.fill();
  }

  function drawLine(p1, p2) {
    ctx.beginPath();
    ctx.moveTo(p1.x*width, p1.y*height);
    ctx.lineTo(p2.x*width, p2.y*height);
    ctx.strokeStyle = "yellow";
    ctx.stroke();

  }

  function transformX(x){
    let rect = canvas.getBoundingClientRect();
    let scaleX = canvas.width/rect.width;
    return (x - rect.left) * scaleX;
  }

  function transformY(y){
      let rect = canvas.getBoundingClientRect();
      let scaleY = canvas.height/rect.height;
      return (y - rect.top) * scaleY;
    }

  return {init: init};
}());
