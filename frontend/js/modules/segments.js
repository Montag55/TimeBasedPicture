let Segments = (function () {
    let segments,
        count,
        asideWrapper,
        mainWrapper,
        addButton,
        down = false,
        downX = 0,
        oldX = 0,
        mouseId = 0,
        mouseTarget = '',
        mouseSegment = null,
        updateFlag = false,
        activeAside = null,
        activeMain = null,
        mask = null,
        maskCtx = null,
        enableMask = null,
        showMask = null,
        brushSize = null,
        left = null,
        right = null,
        change = null;

    function init(width, height) {
        segments = {};
        count = 0;
        asideWrapper = document.querySelector('.aside .scroll-wrapper');
        mainWrapper = document.querySelector('.main .scroll-wrapper');
        addButton = document.querySelector('.aside .video');

        mask = document.querySelector('.mask');
        mask.width = width;
        mask.height = height;
        maskCtx = mask.getContext('2d');

        enableMask = document.querySelector('.enableMask');
        showMask = document.querySelector('.showMask');
        brushSize = document.querySelector('.brushSize');

        showMask.addEventListener('change', function () {
            mask.style.display = (showMask.checked) ? 'block' : 'none';
        });

        enableMask.addEventListener('change', function () {
            segments[Edit.getActive()].enableMask = enableMask.checked;
            requestUpdate(segments[Edit.getActive()]);
        });
        mask.addEventListener('mousedown', function (e) {
            left = e.button === 0;
            right = e.button === 2;
        });

        mask.addEventListener('mouseup', function () {
            left = false;
            right = false;
        });

        mask.addEventListener('mousemove', function (e) {
            if (left) {
                change = true;
                maskCtx.globalCompositeOperation = 'source-over';
                maskCtx.fillStyle = 'white';
                maskCtx.beginPath();
                let p = getScale({x: e.pageX, y: e.pageY});
                maskCtx.arc(p.x, p.y, brushSize.value, 0, 2 * Math.PI);
                maskCtx.fill();
            } else if (right) {
                change = true;
                maskCtx.globalCompositeOperation = 'destination-out';
                maskCtx.beginPath();
                let p = getScale({x: e.pageX, y: e.pageY});
                maskCtx.arc(p.x, p.y, brushSize.value, 0, Math.PI * 2);
                maskCtx.fill();
            }
        });

        window.addEventListener('mouseup', function (e) {
            if (change === true) {
                segments[Edit.getActive()].imageData = maskCtx.getImageData(0, 0, mask.width, mask.height);
                let dataUrl = mask.toDataURL('image/png');
                let base64Data = dataUrl.replace(/^data:image\/png;base64,/, "");
                fs.writeFile(`mask${Edit.getActive()}.png`, base64Data, 'base64', function (err) {
                    console.log(err);
                });
                requestUpdate(segments[Edit.getActive()]);
            }
            change = false;
        });

        addButton.addEventListener('click', function () {
            requestAdd();
        });
        window.addEventListener('mouseup', function () {
            down = false;
            if (updateFlag === true) {
                requestUpdate(segments[mouseId]);
            }
            updateFlag = false;
        });

        window.addEventListener('mousemove', mousemove);
    }

    function getScale(pos) {
        let rect = mask.getBoundingClientRect();
        let scaleX = mask.width / rect.width;
        let scaleY = mask.height / rect.height;
        let x = scaleX * (pos.x - rect.left);
        let y = scaleY * (pos.y - rect.top);
        return {
            x: x,
            y: y
        };
    }

    function loadMask(segment) {
        maskCtx.putImageData(segment.imageData, 0, 0);
    }

    function addSegment(id) {
        showMask.checked = false;
        enableMask.checked = false;

        let aside = `<div class="segment">
            <div class="name">
                segment_${id}
            </div>
            <div class="del">
                <i class="fas fa-trash"></i>
            </div>
            <div class="toggle">
                <i class="fas fa-toggle-on"></i>
            </div>
            <div class="backdrop">
                <i class="fas fa-puzzle-piece"></i>
            </div>
        </div>`;
        aside = new DOMParser().parseFromString(aside, 'text/html').children[0].children[1].children[0];

        aside.addEventListener('click', function () {
            Edit.show(id);
            loadMask(segments[id]);
        });

        let toggle = aside.querySelector('.toggle');
        let toggleIcon = toggle.querySelector('i');

        toggle.addEventListener('click', function () {
            if (aside.classList.contains('deactive')) {
                aside.classList.remove('deactive');
                main.classList.remove('deactive');
                toggleIcon.classList.add('fa-toggle-on');
                toggleIcon.classList.remove('fa-toggle-off');
                segment.global_i = 1;
            } else {
                aside.classList.add('deactive');
                main.classList.add('deactive');
                toggleIcon.classList.add('fa-toggle-off');
                toggleIcon.classList.remove('fa-toggle-on');
                segment.global_i = 0;
            }
            requestUpdate(segment);
        });


        aside.querySelector('.del').addEventListener('click', function () {
            console.log(id + 'listener');
            remove(id);
        });



        let main = `<div class="segment">
            <div class="block">
                <div class="bar">segment_${id}</div>
                <div class="graph">
                    <div class="progress"></div>
                </div>
                <div class="scale-left"></div>
                <div class="scale-right"></div>
            </div>
        </div>`;
        main = new DOMParser().parseFromString(main, 'text/html').children[0].children[1].children[0];

        let block = main.querySelector('.block');
        let bar = block.querySelector('.bar');
        let left = block.querySelector('.scale-left');
        let right = block.querySelector('.scale-right');
        let progress = block.querySelector('.progress');

        block.addEventListener('click', function () {
            Edit.show(id);
            loadMask(segments[id]);
        });

        bar.addEventListener('mousedown', function (e) {
            down = true;
            mouseId = id;
            mouseSegment = segment;
            downX = getFrame(e.pageX);
            oldX = downX;
            target = 'bar';
            Edit.show(mouseId);
            loadMask(segments[mouseId]);
        });

        left.addEventListener('mousedown', function (e) {
            down = true;
            mouseId = id;
            mouseSegment = segment;
            downX = getFrame(e.pageX);
            oldX = downX;
            target = 'left';
            Edit.show(mouseId);
            loadMask(segments[mouseId]);
        });

        right.addEventListener('mousedown', function (e) {
            down = true;
            mouseId = id;
            mouseSegment = segment;
            downX = getFrame(e.pageX);
            oldX = downX;
            target = 'right';
            Edit.show(mouseId);
            loadMask(segments[mouseId]);
        });

        aside.addEventListener('click', function () {
            if (activeAside !== null) {
                activeAside.classList.remove('active');
                activeMain.classList.remove('active');
            }
            aside.classList.add('active');
            main.classList.add('active');
            activeAside = aside;
            activeMain = main;
        });

        aside.addEventListener('mousedown', function () {
            console.log(mouseId);
            Edit.show(mouseId);
            loadMask(segments[mouseId]);
        });

        main.addEventListener('click', function () {
            if (activeAside !== null) {
                activeAside.classList.remove('active');
                activeMain.classList.remove('active');
            }
            aside.classList.add('active');
            main.classList.add('active');
            activeAside = aside;
            activeMain = main;
        });

        let start = 0;
        let end = 5;

        let segment = {
            id: id,
            aside: aside,
            main: main,
            progress: progress,
            block: block,
            start: start,
            end: end,
            global_i: 1,
            local_i: 1,
            enableMask: false,
            imageData: maskCtx.createImageData(mask.width, mask.height)
        };

        //Save blank mask
        let canvas = document.createElement('canvas');
        canvas.width = mask.width;
        canvas.height = mask.height;
        let dataUrl = canvas.toDataURL('image/png');
        let base64Data = dataUrl.replace(/^data:image\/png;base64,/, "");
        fs.writeFile(`mask${id}.png`, base64Data, 'base64', function (err) {
            console.log(err);
        });

        segments[id] = segment;

        asideWrapper.appendChild(aside);
        mainWrapper.appendChild(main);

        update(id);
        requestUpdate(segments[id]);
        count++;
    }

    function update(id) {
        let segment = segments[id];
        //requestUpdate(segment);
        segment.block.style.left = `calc(var(--frame-width) * ${segment.start})`;
        segment.block.style.width = `calc(var(--frame-width) * ${segment.end - segment.start})`;

    }

    function remove(id) {
        let segment = segments[id];

        asideWrapper.removeChild(segment.aside);
        mainWrapper.removeChild(segment.main);

        segments[id] = null;
        console.log(id, 'remove')
        requestRemove(id);
    }

    function mousemove(e) {
        if (down === false) {
            return;
        }
        let posX = getFrame(e.pageX);

        if (target === 'bar') {
            let length = mouseSegment.end - mouseSegment.start;
            mouseSegment.start += posX - oldX;
            mouseSegment.start = Math.min(Math.max(mouseSegment.start, 0), MAX_FRAMES - length);
            mouseSegment.end = mouseSegment.start + length;
        } else if (target === 'left') {
            mouseSegment.start += posX - oldX;
            mouseSegment.start = Math.min(Math.max(mouseSegment.start, 0), MAX_FRAMES - length);
        } else if (target === 'right') {
            mouseSegment.end += posX - oldX;
            mouseSegment.end = Math.min(mouseSegment.end, MAX_FRAMES);
        }

        updateFlag = true;
        update(mouseId);
        requestUpdate(segments[mouseId]);
        Edit.show(mouseId);
        loadMask(segments[mouseId]);
        oldX = posX;
    }

    function getFrame(x) {
        let rect = mainWrapper.getBoundingClientRect();
        let percent = Math.min(Math.max((x - rect.left) / mainWrapper.offsetWidth, 0), 1);
        return Math.floor(percent * MAX_FRAMES);
    }

    function getSegment(id) {
        return segments[id];
    }

    function setSegment(id, segment) {
        segments[id] = segment;
        update(id);
        requestUpdate(segment);
    }

    function getProgress() {
        if (!segments) {
            return;
        }
        Object.keys(segments).forEach(function (key) {
            if (typeof (segments[key]) === 'undefined' || segments[key] === null) {
                return;
            }
            let segment = segments[key];
            requestProgress(segment.id);
        });
    }

    function updateProgress(data) {
        if (typeof (segments[data.id]) === 'undefined' || segments[data.id] === null) {
            return;
        }
        // segments[data.id].progress.style.width = 100 - parseFloat(data.progress) + "%";
        segments[data.id].progress.style.left = parseFloat(data.progress) + "%";
    }

    return {
        init: init,
        addSegment: addSegment,
        getSegment: getSegment,
        setSegment: setSegment,
        getProgress: getProgress,
        updateProgress: updateProgress
    };
}());