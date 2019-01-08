

let Timeline = (function () {
    let mainVideo,
        timeLabel,
        scrollReset,
        images,
        labels,
        player,
        canvas,
        context,
        active,
        currentFrame,
        paused,
        stepLeft,
        stepRight,
        playBtn,
        pauseBtn,
        stopBtn,
        fakeinput,
        menu;

    menu = document.querySelector('.top-bar .open');
    fakeinput = document.querySelector('.fakeinput');


    function changeInput() {
        let string = fakeinput.files[0].path;
        let split = string.split('/');
        let filename = split[split.length - 1];
        document.querySelector('.video-name').innerText = filename;
        requestInit(string.replace(filename, ''));
        menu.removeEventListener('click', openDialog, true);
    }
    fakeinput.addEventListener('change', changeInput);

    function openDialog() {
        fakeinput.click();
    }
    menu.addEventListener('click', openDialog);

    function init(path, start, end, width, height) {
        MAX_FRAMES = end;
        FILE_PATH = path;
        mainVideo = document.querySelector('.main .scroll-wrapper .video');
        timeLabel = document.querySelector('.main .time-label');
        scrollReset = document.querySelector('.scroll-reset');
        player = document.querySelector('.player');
        canvas = player.querySelector('canvas');
        stepLeft = player.querySelector('.fa-step-backward');
        stepRight = player.querySelector('.fa-step-forward');
        playBtn = player.querySelector('.fa-play');
        pauseBtn = player.querySelector('.fa-pause');
        stopBtn = player.querySelector('.fa-stop');

        stepLeft.addEventListener('click', function () {
            currentFrame -= 1;
            currentFrame = Math.max(currentFrame, 1);
            show(currentFrame);
            paused = true;
        });

        stepRight.addEventListener('click', function () {
            currentFrame += 1;
            currentFrame = Math.min(currentFrame, MAX_FRAMES - 1);
            show(currentFrame);
            paused = true;
        });

        playBtn.addEventListener('click', function () {
            if (paused == true) {
                paused = false;
                play();
            }
        });

        pauseBtn.addEventListener('click', function () {
            paused = true;
        });

        stopBtn.addEventListener('click', function () {
            paused = true;
            currentFrame = 1;
            show(currentFrame);
        });

        context = canvas.getContext('2d');
        canvas.width = width;
        canvas.height = height;

        images = [];
        labels = [];
        active = null;
        initVideo(path);

        ScrollY.init();
        ScrollX.init();
        Segments.init(width, height);

        scrollReset.addEventListener('click', function () {
            ScrollX.reset();
            ScrollY.reset();
        });

        setTimeout(function () {
            show(1);
            ScrollX.set(0, end / 10000);
        }, 1000);


        currentFrame = 0;
        paused = true;
    }
    //init('../videos/bootraffa/');

    function initVideo(path) {
        for (let i = 1; i <= MAX_FRAMES; i++) {
            let tempLabel = document.createElement('P');
            tempLabel.addEventListener('click', function () {
                show(i);
            });
            tempLabel.innerText = i;
            let tempImg = document.createElement('DIV');
            tempImg.classList.add('img');
            let img = document.createElement('IMG');
            tempImg.appendChild(img);
            img.src = `${path}preview/frame${i}.jpg`;
            document.documentElement.style.setProperty('--frame-width', 100 / MAX_FRAMES + '%');
            document.documentElement.style.setProperty('--video-height', 15 + '%');
            mainVideo.appendChild(tempImg);
            timeLabel.appendChild(tempLabel);
            images.push(img);
            labels.push(tempLabel);
        }
        window.addEventListener('load', function () {
            show(1);
        });
    }


    function play() {
        if (paused) {
            return;
        }
        currentFrame += 1;
        if (currentFrame < MAX_FRAMES) {
            //window.requestAnimationFrame(play);
            setTimeout(play, 1000 / 24);
        } else {
            currentFrame = MAX_FRAMES -1;
        }
        show(currentFrame);

    }


    function show(i) {
        if (active !== null) {
            active.classList.remove('active-frame');
        }
        currentFrame = i;
        labels[i - 1].classList.add('active-frame');
        context.drawImage(images[i - 1], 0, 0);
        active = labels[i - 1];
    }


    return {
        init: init
    };
}());
