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
    let interpretations, anchor, typeDom, addDom;

    function init() {
        anchor = document.querySelector('.interpretations');
        typeDom = anchor.querySelector('select');
        addDom = anchor.querySelector('.add');

        addDom.addEventListener('click', add);
    }
    init();

    function add() {
        let type = typeDom.value;
        if (type == "average") {

        } else if (type == "transfer") {

        } else if (type == "overwrite") {

        } else if (type == "motion") {

        }
        console.log(type);
    }

    return {

    };
}());