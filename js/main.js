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
    document.getElementById("user_avatar").style.display = "inline-block";
    document.getElementById("auth").remove();
}

function unauthorized_user() {
    document.getElementById("sign_in").style.display = "inline-block";
    document.getElementById("sign_out").remove();
    document.getElementById("user_name").remove();
    email_is_unknown();
    not_admin();
}

function admin() {
    document.getElementById("settings_page").style.display = "inline-block";
}

function not_admin() {
    document.getElementById("settings_page").remove();
}

function email_is_unknown() {
    document.getElementById("user_avatar").remove();
}

function sign_out() {
    document.cookie = "user_id=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/";
    document.cookie = "session_id=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/";
    return true;
}

function store_user_name() {
    localStorage.setItem("user_name", $("#input_user_name").val());
}

function document_postprocessing() {
    if ($("#input_user_name")) {
        $("#input_user_name").val(localStorage.getItem("user_name"));
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

$(document).on("click", "#close_auth_error", close_auth_error);
$(document).on("click", "#sign_in *", begin_authorization);
$(document).on("click", "#sign_out *", sign_out);
$(document).on("submit", "#auth_form", store_user_name);
$(document).ready(document_postprocessing);

