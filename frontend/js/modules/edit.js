let Edit = (function () {
    let anchor, name, start, end, segment, edit_id, interpretations, global_i, local_i, active;

    function init() {
        anchor = document.querySelector('.edit');
        name = anchor.querySelector('.name');
        start = anchor.querySelector('.start');
        local_i = anchor.querySelector('.localI');
        global_i = anchor.querySelector('.globalI');
        select = anchor.querySelector('select');
        start.step = 1;
        start.max = MAX_FRAMES;
        start.min = 0;
        end = anchor.querySelector('.end');
        end.step = 1;
        end.max = MAX_FRAMES;
        end.min = 1;
        active = 0;

        start.addEventListener('change', update);
        end.addEventListener('change', update);
        global_i.addEventListener('change', update);
        local_i.addEventListener('change', update);
        select.addEventListener('change', function () {
            if (select.value == -1) {
                return;
            }
            Interpretations.connect(active, select.value);
            segment = Segments.getSegment(active);
            segment.interpretation = select.value;
        });
    }

    function show(id) {
        segment = Segments.getSegment(id);
        if (segment) {
            start.value = segment.start;
            end.value = segment.end;
            // global_i = segment.global_i;
            // local_i = segment.local_i;
            global_i.value = segment.global_i;
            local_i.value  = segment.local_i;
            select.value = segment.interpretation || -1;
            edit_id = id;
            name.innerText = 'segment_' + id;
            active = id;
        }
    }

    function update() {
        segment.start = parseInt(start.value);
        segment.end = parseInt(end.value);
        segment.global_i = parseInt(global_i.value);
        segment.local_i = parseInt(local_i.value);
        Segments.setSegment(edit_id,  segment);
    }

    function addInterpretation(name, id) {
        let option = document.createElement('option');
        option.innerText = name;
        option.value = id;
        select.appendChild(option);
    }

    function updateInterpretation(name, id) {
        select.querySelectorAll('option').forEach(function (entry) {
            if (entry.value == id) {
                entry.innerText = name;
            }
        });
    }

    function getActive() {
        return active;
    }

    return {
        init: init,
        show: show,
        addInterpretation: addInterpretation,
        updateInterpretation: updateInterpretation,
        getActive: getActive
    };
}());
