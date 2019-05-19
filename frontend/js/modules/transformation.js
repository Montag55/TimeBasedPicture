//Bei edit segment global_i (0 - 10) local_i (0 - 10) --> slider
//Add transformation (typ vorher festlegen)
//AVG(type==0) nur ID
//Transfer(type==1) {}
//Overpaint(type==2) {referenzID(segment), threshold(int0-255), offset(int), gapping(int)}
//Motion(type==3) {referenzID(segment), threshold(int0-255)}

//Connect transformation

//Open file
//folder/frame${count}.jpg

//

let Transformations = (function () {
    let transformations, anchor, select, editor, instances, addDom, container, active, activeId;

    function init() {
        anchor = document.querySelector('.transformations');
        select = anchor.querySelector('select');
        editor = anchor.querySelector('.editor');
        addDom = anchor.querySelector('.add');
        addDom.addEventListener('click', add);
        container = anchor.querySelector('.container');

        active = null;
        activeId = -1;

        instances = [];
        transformations = {};

        Loader.getJSON('transformations.json', function (obj) {
            initTransformations(obj);
        });
    }
    init();

    function initTransformations(config) {
        config.forEach(function (transformation) {
            let parent = document.createElement('div');
            transformations[transformation.name] = {};
            let transformation_object = transformations[transformation.name];
            transformation_object.dom = parent;

            transformation_object.valueFunctions = [];
            transformation_object.setFunctions = [];

            parent.classList.add('inter');
            parent.id = transformation.name;
            let heading = document.createElement('h1');
            heading.innerHTML = `<b>${transformation.name}</b><br>`;
            parent.appendChild(heading);
            let instance_name = document.createElement('span');
            instance_name.classList.add('instance_name');
            heading.appendChild(instance_name);

            let option = document.createElement('option');
            option.innerText = transformation.name;
            select.appendChild(option);

            transformation_object.id = transformation.id;

            transformation.types.forEach(function (type, index) {
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
                    transformation_object.valueFunctions.push(function () {
                        return [slider.value];
                    });
                    transformation_object.setFunctions.push(function (v) {
                        slider.value = v[index];
                    });
                }else if (type.type === 'number_edit') {
                    let edit = document.createElement('input');
                    edit.type = 'number';
                    edit.min = type.min;
                    edit.max = type.max;
                    edit.step = type.step;
                    edit.name = type.name;
                    edit.value = 0;
                    parent.appendChild(edit);
                    edit.addEventListener('change', function () {
                        update();
                    });
                    transformation_object.valueFunctions.push(function () {
                        return [edit.value];
                    });
                    transformation_object.setFunctions.push(function (v) {
                        edit.value = v[index];
                    });
                } else if (type.type === 'select') {
                    let edit = document.createElement('select');
                    edit.name = type.name;
                    parent.appendChild(edit);
                    type.options.forEach(function (name, index) {
                        let option = document.createElement('option');
                        option.innerText = name;
                        option.value = index;
                        edit.appendChild(option);
                    });
                    edit.addEventListener('change', function () {
                        update();
                    });
                    transformation_object.valueFunctions.push(function () {
                        return [edit.value];
                    })
                    transformation_object.setFunctions.push(function (v) {
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
                    transformation_object.valueFunctions.push(function () {
                        return [text.value];
                    });
                    transformation_object.setFunctions.push(function (v) {
                        text.value = v[index];
                    });
                } else if (type.type === 'transfer') {
                    let transfer = new TransferFunction(200, 200);
                    parent.appendChild(transfer.canvas);
                    transformation_object.valueFunctions.push(function () {
                        return transfer.getValues();
                    });
                    transformation_object.setFunctions.push(function (v) {
                        transfer.setValues(v.splice(index, v.length));
                    });
                    transfer.onChange(function () {
                        update();
                    });

                } else if (type.type === 'color') {
                    let color = new ColorPicker([document.querySelector('.image img'), document.querySelector('.player').querySelector('canvas')]);
                    parent.appendChild(color.dom);
                    transformation_object.valueFunctions.push(function () {
                        return color.getValues();
                    });
                    transformation_object.setFunctions.push(function (v) {
                        color.setValues(v.splice(index, v.length));
                    });
                    color.onChange(function () {
                        update();
                    });
                } else if (type.type === 'points') {
                    let points = new PointPicker([document.querySelector('.image img'), document.querySelector('.player').querySelector('canvas')]);
                    parent.appendChild(points.dom);
                    transformation_object.valueFunctions.push(function () {
                        return points.getValues();
                    });
                    transformation_object.setFunctions.push(function (v) {
                        points.setValues(v.splice(index, v.length));
                    });
                    points.onChange(function () {
                        update();
                    });
                } else if (type.type === 'mask') {
                    let mask = new PaintMask(document.querySelector('.paint'));
                    parent.appendChild(mask.dom);
                    transformation_object.valueFunctions.push(function () {
                        return mask.getValues();
                    });
                    transformation_object.setFunctions.push(function (v) {
                        mask.setValues(v.splice(index, v.length));
                    });
                    mask.onChange(function () {
                        update();
                    });
                    transformation_object.getImageData = function () { return mask.getImageData()};
                    transformation_object.setImageData = function (d) {mask.setImageData(d)};
                }
            });
            transformation_object.getValues = function () {
                let arr = [];
                transformation_object.valueFunctions.forEach(function (f) {
                    arr = arr.concat(f());
                });
                return arr;
            };
            transformation_object.setValues = function (v) {
                transformation_object.setFunctions.forEach(function (f) {
                    f(v);
                });
            }
            editor.appendChild(parent);
        });
    }

    function add() {
        let values = transformations[select.value].getValues();
        let type = select.value;
        instances.push({
            type: type,
            values: values
        });
        let temp = transformations[select.value].getValues();
        temp.unshift(transformations[select.value].id);
        requestAddTransformation.apply(null, temp);
    }

    function update() {
        let instance = instances[activeId];
        let transformation = transformations[instance.type];
        instance.values = transformation.getValues();
        if (instance.type === 'Paint') {
            instance.imageData = transformation.getImageData();
        }
        let temp = transformation.getValues();
        temp.unshift(activeId);
        requestManipulateTransformation.apply(null, temp);
    }

    function addId(id) {
        let instance = instances[instances.length - 1];
        instance.id = id;
        let div = document.createElement('div');
        div.classList.add('instance');
        div.innerText = `transformation_${id}`;
        div.contentEditable = true;
        instance.name = div.innerText;
        container.appendChild(div);
        div.addEventListener('click', function () {
            load(instance);
        });

        div.addEventListener('keyup', function () {
            instance.name = this.innerText;
            let transformation = transformations[instance.type];
            if (transformation.dom == active) {
                transformation.dom.querySelector('.instance_name').innerText = instance.name;
            }
            Edit.updateTransformation(this.innerText, id);
        });
        load(instance);
        Edit.addTransformation(div.innerText, id);
    }

    function load(instance) {
        if (active !== null) {
            active.classList.remove('active');
        }

        let transformation = transformations[instance.type];
        active = transformation.dom;
        transformation.dom.querySelector('.instance_name').innerText = instance.name;
        activeId = instance.id;
        active.classList.add('active');
        if (instance.type === 'Paint') {
            transformation.setImageData(instance.imageData);
        }
        transformation.setValues(instance.values);
    }

    function connect(segmentId, transformationId) {
        requestConnect(segmentId, transformationId);
    }

    function getActive() {
        return activeId;
    }

    return {
        addId: addId,
        connect: connect,
        update: update,
        getActive: getActive
    };
}());
