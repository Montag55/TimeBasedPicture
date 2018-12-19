let Masking = (function () {
    let canvas, context, down, toolsDom, tools, active, checkbox;

    function init() {
        return;
        canvas = document.querySelector('.mask');
        context = canvas.getContext('2d');

        canvas.width = 1920;
        canvas.height = 1080;

        canvas.addEventListener('mousedown', mousedown);
        canvas.addEventListener('mouseup', mouseup);
        canvas.addEventListener('mousemove', mousemove);

        toolsDom = document.querySelector('.tools');

        tools = toolsDom.querySelectorAll('i');
        tools.forEach(function (entry, index) {
            entry.addEventListener('click', function () {
                show(index);
            });
        });

        checkbox = document.querySelector('.masking input[type=checkbox]');
        checkbox.addEventListener('click', function () {
            if (checkbox.checked) {
                canvas.style.opacity = 0.5;

            } else {
                canvas.style.opacity = 0;

            }
        });

        let downPos =  {x: 0, y: 0};

        active = 0;
        down = false;
        context.fillStyle = 'red';

    }

    function show(index) {
        tools[active].classList.remove('active');
        tools[index].classList.add('active');
        active = index;
    }

    function mousedown(e) {
        down = true;
        downPos = getScale({x: e.clientX, y: e.clientY});
    }

    function getScale(pos) {
        let rect = canvas.getBoundingClientRect();
        let scaleX = canvas.width / rect.width;
        let scaleY = canvas.height / rect.height;
        let x = scaleX * (pos.x - rect.left);
        let y = scaleY * (pos.y - rect.top);
        return {
            x: x,
            y: y
        };
    }

    function mousemove(e) {
        if (down === false) {
            return;
        }
        let pos = getScale({x: e.clientX, y: e.clientY});
        if (active === 0) {
            context.fillRect(downPos.x, downPos.y, pos.x - downPos.x, pos.y - downPos.y);
        } else if (active === 1) {
            context.beginPath();
            context.arc(pos.x, pos.y, 20, 0, 2 * Math.PI);
            context.fill();
        }
        
    }

    function mouseup() {
        down = false;
    }

    return {
        init: init
    };
}());