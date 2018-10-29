let Tabs = (function () {
    let buttonsDom, buttons, tabsDom, tabs, active;


    function init() {
        buttonsDom = document.querySelector('.tab-buttons');
        tabsDom = document.querySelector('.tabs');
        buttons = buttonsDom.querySelectorAll('.tab-button');
        tabs = tabsDom.querySelectorAll('.tab');

        buttons.forEach(function (entry, index) {
            entry.addEventListener('click', function () {
                show(index);
            });
        });

        active = 0;
        show(0);
    }

    function show(index) {
        buttons[active].classList.remove('active');
        tabs[active].classList.remove('active');
        buttons[index].classList.add('active');
        tabs[index].classList.add('active');
        active = index;
    }

    return {
        init: init
    };
}());