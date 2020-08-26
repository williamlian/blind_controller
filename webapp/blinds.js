var url = "/pubsub/publish/723blinds";
var BEDROOM = 15;
var LIVING = 14;

function getBody(chan, btn) {
    return `{"channel":${chan},"button":"${btn}"}`;
}

function send(chan, btn) {
    var url = getBody(chan, btn);
    $.post(path, body, function(json){
        console.log("Done");
    }).fail(function(jqxhr, textStatus, error){
        window.alert("Failed to operate: " + jqxhr.status + ". " + error);
    }, 'json');
}
