let Edit = (function () {
    let anchor, name, start, end, segment, edit_id;

    function init() {
        anchor = document.querySelector('.edit');
        name = anchor.querySelector('.name');
        start = anchor.querySelector('.start');
        start.step = 1;
        start.max = MAX_FRAMES;
        start.min = 0;
        end = anchor.querySelector('.end');
        end.step = 1;
        end.max = MAX_FRAMES;
        end.min = 0;

        start.addEventListener('change', update);
        end.addEventListener('change', update);
    }

    function show(id) {
        segment = Segments.getSegment(id);
        if (segment) {
            start.value = segment.start;
            end.value = segment.end;
            edit_id = id;
            name.innerText = 'segment_' + id;
        }
        
    }

    function update() {
        segment.start = start.value;
        segment.end = end.value;
        Segments.setSegment(edit_id,  segment);
    }

    return {
        init: init,
        show: show
    };
}());