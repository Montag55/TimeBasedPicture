

let Preview = (function () {
    let preview, canvas, context, imageData, time, spinner;

    function init() {
        Edit.init();
        Tabs.init();
        Masking.init();
        preview = document.querySelector('.image img');

        time = document.querySelector('.time');

        spinner = document.querySelector('.spinner');


        hideSpinner();

    }
    init();

    function draw(data, ms) {
        //imageData.data.set(data);
        //context.putImageData(imageData, 0, 0);
        //time.innerText = ms;
    }

    let then = Date.now();
    window.requestAnimationFrame(loop);
    function loop() {


        if (Date.now() - then > 200) {
            preview.src = `../state_tmp.png?${Date.now()}`;
            Segments.getProgress();
        }
        window.requestAnimationFrame(loop);
    }

    function showSpinner() {
        spinner.style.opacity = 0.5;
    }

    function hideSpinner() {
        spinner.style.opacity = 0.0;
    }

    

    return {
        init: init,
        draw: draw,
        showSpinner: showSpinner,
        hideSpinner: hideSpinner
    };
}());
