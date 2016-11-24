// License: MIT (Expat)

"use strict";

function set_focus(item) {
    document.getElementById(item).focus();
    document.getElementById(item).scrollIntoView();
}

function close_pass_update_error() {
    $("#pass_update_error").hide();
    set_focus("current_password");
}

function close_pass_update_success() {
    $("#pass_update_success").hide();
    set_focus("current_password");
}

function close_all_messages() {
    close_pass_update_error();
    close_pass_update_success();
}

function redirect() {
    if (localStorage["force_pass_update"]) {
        localStorage.removeItem("force_pass_update");
    }
    if (localStorage["redirect_to"]) {
        var url = localStorage.redirect_to;
        localStorage.removeItem("redirect_to");
        location.href = url;
    }
}

function password_update_result(data) {
    var obj = $.parseJSON(data);

    if (!obj.hasOwnProperty("status"))
        return;

    if (obj.status === "success") {
        $("#pass_update_success").show();
        setTimeout(redirect, 6000);
    }
    else {
        var error = $("#" + obj.status).text();
        $("#pass_update_error_label").html(error);
        $("#pass_update_error").show();
    }
}

function submit_pass_update_form(event) {
    event.preventDefault();
    close_all_messages();

    var list = ["current_password", "new_password", "confirm_password"];
    for (var k in list) {
        if (!$("#" + list[k]).val().trim()) {
            set_focus(list[k]);
            return;
        }
    }

    var data = {
        "current_password": $("#current_password").val(),
        "new_password": $("#new_password").val(),
        "confirm_password": $("#confirm_password").val()
    };
    $.post("?ajax=password_update", JSON.stringify(data), password_update_result);
}

$(document).on("click", "#close_pass_update_error", close_pass_update_error);
$(document).on("click", "#close_pass_update_success", close_pass_update_success);
$(document).on("submit", "#pass_update_form", submit_pass_update_form);

