// License: MIT (Expat)

"use strict";

function begin_authorization() {
    document.getElementById("sign_in").style.display = "none";
    document.getElementById("content").style.display = "none";
    document.getElementById("auth").style.display = "flex";
    return false;
}

function authorized_user() {
    document.getElementById("sign_in").remove();
    document.getElementById("sign_out").style.display = "inline-block";
    document.getElementById("user_name").style.display = "inline-block";
    document.getElementById("auth").remove();
}

function unauthorized_user() {
    document.getElementById("sign_in").style.display = "inline-block";
    document.getElementById("sign_out").remove();
    document.getElementById("user_name").remove();
    remove_user_avatar();
    not_admin();
}

function admin() {
    document.getElementById("settings_page").style.display = "inline-block";
}

function not_admin() {
    document.getElementById("settings_page").remove();
}

function show_user_avatar() {
    document.getElementById("user_avatar").style.display = "inline-block";
}

function remove_user_avatar() {
    document.getElementById("user_avatar").remove();
}

function sign_out() {
    $.post("?ajax=sign_out");
    return true;
}

function store_user_name() {
    localStorage.setItem("user_name", $("#input_user_name").val());
    $("#button_submit_auth_form").css("cursor", "wait");
}

function document_postprocessing() {
    if ($("#input_user_name")) {
        $("#input_user_name").val(localStorage.getItem("user_name"));
    }
    if ($("#user_avatar").find("img").attr("src")) {
        if ($("#user_avatar").find("img").attr("src").trim()) {
            show_user_avatar();
        }
        else {
            remove_user_avatar();
        }
    }
}

function show_auth_error() {
    document.getElementById("auth_error").style.display = "table";
}

function close_auth_error() {
    document.getElementById("auth_error").style.display = "none";
    document.getElementById("input_user_name").focus();
    document.getElementById("input_user_name").scrollIntoView();
}

function force_password_update() {
    var currentUrl = $("#sign_out").find("a").attr("href");
    var redirectTo = $("#user_name").find("a").attr("href") + "/security";
    if (currentUrl !== redirectTo) {
        localStorage.redirect_to = currentUrl;
        localStorage.force_pass_update = true;
        location.href = redirectTo;
    }
}

$(document).on("click", "#close_auth_error", close_auth_error);
$(document).on("click", "#sign_in *", begin_authorization);
$(document).on("click", "#sign_out *", sign_out);
$(document).on("submit", "#auth_form", store_user_name);
$(document).ready(document_postprocessing);

