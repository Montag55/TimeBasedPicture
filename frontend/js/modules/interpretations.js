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
    let interpretations, anchor, select, editor, instances, id, addDom;

    function init() {
        anchor = document.querySelector('.interpretations');
        select = anchor.querySelector('select');
        editor = anchor.querySelector('.editor');
        addDom = anchor.querySelector('.add');
        addDom.addEventListener('click', add);

        id = 0;
        
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
                if (type.type === 'number') {
                    let slider = document.createElement('input');
                    slider.type = 'range';
                    slider.min = type.min;
                    slider.max = type.max;
                    slider.step = type.step;
                    slider.name = type.name;
                    parent.appendChild(slider);
                    interpretation_object.valueFunctions.push(function () {
                        return [slider.value];
                    });
                    interpretation_object.setFunctions.push(function (v) {
                        slider.value = v[index];
                    });
                } else if (type.type === 'string') {
                    let text = document.createElement('input');
                    text.type = 'text';
                    text.name = type.name;
                    parent.appendChild(text);
                    interpretation_object.valueFunctions.push(function () {
                        return [text.value];
                    });
                    interpretation_object.setFunctions.push(function (v) {
                        slider.value = v[index];
                    });
                } else if (type.type === 'transfer') {
                    let transfer = new TransferFunction(200, 200);
                    parent.appendChild(transfer.canvas);
                    interpretation_object.valueFunctions.push(function () {
                        return transfer.getValues();
                    });
                    interpretation_object.setFunctions.push(function (v) {
                        transfer.setValues(v.splice(index, v.length));
                    })
                } else if (type.type === 'color') {
                    let color = new ColorPicker([document.querySelector('.image img'), document.querySelector('.player').querySelector('canvas')]);
                    parent.appendChild(color.dom);
                    interpretation_object.valueFunctions.push(function () {
                        return color.getValues();
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
            console.log(interpretations);
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

    return {
        add: add
    };
}());