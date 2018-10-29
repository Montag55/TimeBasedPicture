let ScrollY = (function () {
    let anchor = document.querySelector('.scroll-y'),
        thumb = anchor.querySelector('.scroll-y-thumb'),
        track = anchor.querySelector('.scroll-y-track'),
        scaleUp = thumb.querySelector('.scale-up'),
        scaleDown = thumb.querySelector('.scale-down'),
        scrollUp = anchor.querySelector('.scroll-up'),
        scrollDown = anchor.querySelector('.scroll-down'),
        wrapper = document.querySelector('.main .scroll-wrapper'),
        asideWrapper = document.querySelector('.aside .scroll-wrapper'),
        video = document.querySelector('.main .video'),
        asideVideo = document.querySelector('.aside .video'),
        down = false,
        downY = 0,
        top = 0,
        scale = 1,
        oldY = 0;


    function init() {
        update();
        
        window.addEventListener('mousemove', mousemove);
        thumb.addEventListener('mousedown', mousedown);
        window.addEventListener('mouseup', mouseup);

        scrollUp.addEventListener('click', function () {
            top -= 0.1;
            update();
        });
        scrollDown.addEventListener('click', function () {
            top += 0.1;
            update();
        })
    }

    function mousedown(e) {
        downY = getScrollYScale(e.pageY);
        oldY = downY;
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

        let posY = getScrollYScale(e.pageY);

        if (target === thumb) {
            top += posY - oldY;
        } else if (target === scaleDown) {
            scale += posY - oldY;
        } else if (target === scaleUp) {
            top += posY - oldY;
            scale -= posY - oldY;
        }


        update();
        oldY = posY;
    }

    function update() {
        scale = Math.min(Math.max(scale, MIN_SCROLL_Y), 1);
        top = Math.min(Math.max(top, 0), 1 - scale);

        let scaleStyle = scale * 100;
        let topStyle = top / scale * 100;
        thumb.style.height = scaleStyle + '%';
        wrapper.style.height = 100 / scaleStyle * 100 + '%';
        asideWrapper.style.height = wrapper.style.height;
        thumb.style.top = top * 100 + '%';
        wrapper.style.top = - topStyle + '%';
        asideWrapper.style.top = wrapper.style.top;

        let videoHeight = parseInt(document.documentElement.style.getPropertyValue('--video-height'));
        console.log(videoHeight);
        video.style.height = videoHeight * scale + '%';
        asideVideo.style.height = videoHeight * scale + '%';
    }


    function getScrollYScale(y) {
        let rect = track.getBoundingClientRect();
        return Math.min(Math.max((y - rect.top) / track.offsetHeight, 0), 1);
    }

    function reset() {
        scale = 1;
        top = 0;
        update();
    }

    return {
        init: init,
        reset: reset
    };
}());