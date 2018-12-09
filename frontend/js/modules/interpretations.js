//Bei edit segment global_i (0 - 10) local_i (0 - 10) --> slider
//Add interpretation (typ vorher festlegen)
//AVG(type==0) nur ID
//Transfer(type==1) {}
//Overpaint(type==2) {referenzID(segment), threshold(int0-255), offset(int), gapping(int)}
//Motion(type==3) {referenzID(segment), threshold(int0-255)}

//Connect interpretation

//Open file
//folder/frame${count}.jpg

//

let Interpretations = (function () {
    let interpretations, anchor, select, editor, instances, addDom, container, active, activeId;

    function init() {
        anchor = document.querySelector('.interpretations');
        select = anchor.querySelector('select');
        editor = anchor.querySelector('.editor');
        addDom = anchor.querySelector('.add');
        addDom.addEventListener('click', add);
        container = anchor.querySelector('.container');

        active = null;
        activeId = -1;

        instances = [];
        interpretations = {};

        Loader.getJSON('interpretations.json', function (obj) {
            initInterpretaions(obj);
        });
    }
    init();

    function initInterpretaions(config) {
        config.forEach(function (interpretation) {
            let parent = document.createElement('div');
            interpretations[interpretation.name] = {};
            let interpretation_object = interpretations[interpretation.name];
            interpretation_object.dom = parent;

            interpretation_object.valueFunctions = [];
            interpretation_object.setFunctions = [];

            parent.classList.add('inter');
            parent.id = interpretation.name;
            let heading = document.createElement('h1');
            heading.innerText = interpretation.name;
            parent.appendChild(heading);

            let option = document.createElement('option');
            option.innerText = interpretation.name;
            select.appendChild(option);

            interpretation_object.id = interpretation.id;

            interpretation.types.forEach(function (type, index) {
                let label = document.createElement('p');
                label.innerText = type.name;
                parent.appendChild(label);
                if (type.type === 'number_slider') {
                    let slider = document.createElement('input');
                    slider.type = 'range';
                    slider.min = type.min;
                    slider.max = type.max;
                    slider.step = type.step;
                    slider.name = type.name;
                    parent.appendChild(slider);
                    slider.addEventListener('change', function () {
                        update();
                    });
                    interpretation_object.valueFunctions.push(function () {
                        return [slider.value];
                    });
                    interpretation_object.setFunctions.push(function (v) {
                        slider.value = v[index];
                    });
                }else if (type.type === 'number_edit') {
                    let edit = document.createElement('input');
                    edit.type = 'number';
                    edit.min = type.min;
                    edit.max = type.max;
                    edit.step = type.step;
                    edit.name = type.name;
                    parent.appendChild(edit);
                    edit.addEventListener('change', function () {
                        update();
                    });
                    interpretation_object.valueFunctions.push(function () {
                        return [edit.value];
                    });
                    interpretation_object.setFunctions.push(function (v) {
                        edit.value = v[index];
                    });
                } else if (type.type === 'string') {
                    let text = document.createElement('input');
                    text.type = 'text';
                    text.name = type.name;
                    parent.appendChild(text);
                    text.addEventListener('change', function () {
                        update();
                    });
                    interpretation_object.valueFunctions.push(function () {
                        return [text.value];
                    });
                    interpretation_object.setFunctions.push(function (v) {
                        text.value = v[index];
                    });
                } else if (type.type === 'transfer') {
                    let transfer = new TransferFunction(200, 200);
                    parent.appendChild(transfer.canvas);
                    interpretation_object.valueFunctions.push(function () {
                        return transfer.getValues();
                    });
                    interpretation_object.setFunctions.push(function (v) {
                        transfer.setValues(v.splice(index, v.length));
                    });
                    transfer.onChange(function () {
                        update();
                    });

                } else if (type.type === 'color') {
                    let color = new ColorPicker([document.querySelector('.image img'), document.querySelector('.player').querySelector('canvas')]);
                    parent.appendChild(color.dom);
                    interpretation_object.valueFunctions.push(function () {
                        return color.getValues();
                    });
                    interpretation_object.setFunctions.push(function (v) {
                        return color.setValues(v.splice(index, v.length));
                    });
                    color.onChange(function () {
                        update();
                    });
                }
            });
            interpretation_object.getValues = function () {
                let arr = [];
                interpretation_object.valueFunctions.forEach(function (f) {
                    arr = arr.concat(f());
                });
                return arr;
            };
            interpretation_object.setValues = function (v) {
                interpretation_object.setFunctions.forEach(function (f) {
                    f(v);
                });
            }
            editor.appendChild(parent);
        });
    }

    function add() {
        let values = interpretations[select.value].getValues();
        let type = select.value;
        instances.push({
            type: type,
            values: values
        });
        let temp = interpretations[select.value].getValues();
        temp.unshift(interpretations[select.value].id);
        requestAddInterpretation.apply(null, temp);
    }

    function update() {
        let instance = instances[activeId];
        let interpretation = interpretations[instance.type];
        instance.values = interpretation.getValues();
        let temp = interpretation.getValues();
        temp.unshift(activeId);
        requestManipulateInterpretation.apply(null, temp);
    }

    function addId(id) {
        let instance = instances[instances.length - 1];
        instance.id = id;
        let div = document.createElement('div');
        div.classList.add('instance');
        div.innerText = `interpretation_${id}`;
        div.contentEditable = true;
        container.appendChild(div);
        div.addEventListener('click', function () {
            load(instance);
        });

        div.addEventListener('keyup', function () {
            Edit.updateInterpretation(this.innerText, id);
        });
        load(instance);
        Edit.addInterpretation(div.innerText, id);
    }

    function load(instance) {
        if (active !== null) {
            active.classList.remove('active');
        }

        let interpretation = interpretations[instance.type];
        active = interpretation.dom;
        activeId = instance.id;
        active.classList.add('active');
        interpretation.setValues(instance.values);
    }

    function connect(segmentId, interpretationId) {
        requestConnect(segmentId, interpretationId);
    }

    return {
        addId: addId,
        connect: connect
    };
}());
