let ScrollX = (function () {
    let anchor = document.querySelector('.scroll-x'),
        thumb = anchor.querySelector('.scroll-x-thumb'),
        track = anchor.querySelector('.scroll-x-track'),
        scaleLeft = thumb.querySelector('.scale-left'),
        scaleRight = thumb.querySelector('.scale-right'),
        scrollLeft = anchor.querySelector('.scroll-left'),
        scrollRight = anchor.querySelector('.scroll-right'),
        wrapper = document.querySelector('.main .scroll-wrapper'),
        down = false,
        downX = 0,
        left = 0,
        scale = 1,
        oldX = 0;

    function init() {
        update();
        
        window.addEventListener('mousemove', mousemove);
        thumb.addEventListener('mousedown', mousedown);
        window.addEventListener('mouseup', mouseup);
        scrollLeft.addEventListener('click', function () {
            left -= 0.1;
            update();
        });
        scrollRight.addEventListener('click', function () {
            left += 0.1;
            update();
        });
    }

    function mousedown(e) {
        downX = getScrollXScale(e.pageX);
        oldX = downX;
        target = e.target;
        down = true;
    }

    function mouseup() {
        down = false;
    }

    function mousemove(e) {
        if (down === false) {
            return;
        }

        let posX = getScrollXScale(e.pageX);

        if (target === thumb) {
            left += posX - oldX;
        } else if (target === scaleRight) {
            scale += posX - oldX;
        } else if (target === scaleLeft) {
            left += posX - oldX;
            scale -= posX - oldX;
        }


        update();
        oldX = posX;
    }

    function update() {
        scale = Math.min(Math.max(scale, MIN_SCROLL_X), 1);
        left = Math.min(Math.max(left, 0), 1 - scale);

        let scaleStyle = scale * 100;
        let leftStyle = left / scale * 100;

        thumb.style.width = scaleStyle + '%';
        wrapper.style.width = 100 / scaleStyle * 100 + '%';

        let labelSize = 100 / scaleStyle * 100 / MAX_FRAMES;

        wrapper.classList.remove('label-1');
        wrapper.classList.remove('label-2');
        wrapper.classList.remove('label-4');
        wrapper.classList.remove('label-8');
        wrapper.classList.remove('label-16');
        if (labelSize > 2.4) {
            wrapper.classList.add('label-1');
        } else if (labelSize > 1.3) {
            wrapper.classList.add('label-2');
        } else if (labelSize > 0.8) {
            wrapper.classList.add('label-4');
        } else if (labelSize > 0.4) {
            wrapper.classList.add('label-8');
        } else {
            wrapper.classList.add('label-16');
        }
    

        thumb.style.left = left * 100 + '%';
        wrapper.style.left = - leftStyle + '%';
    }

    function getScrollXScale(x) {
        let rect = track.getBoundingClientRect();
        return Math.min(Math.max((x - rect.left) / track.offsetWidth, 0), 1);
    }

    function reset() {
        scale = 1;
        left = 0;
        update();
    }

    function set(new_left, new_scale) {
        left = new_left;
        scale = new_scale;
        update();
    }

    return {
        init: init,
        reset: reset,
        set: set
    };
}());