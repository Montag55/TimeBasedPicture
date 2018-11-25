let Loader = (function () {

    function get(url, callback) {
        let xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        xhr.onreadystatechange = function () {
            if (xhr.readyState === 4 && xhr.status === 200) {
                callback(xhr.responseText);
            }
        };
        xhr.send(null);
    }

    function getJSON(url, callback) {
        get(url, function (responseText) {
            callback(JSON.parse(responseText));
        });
    }

    return {
        get: get,
        getJSON: getJSON
    };
}());