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
    $.post("?ajax=sign_out");
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

function redirect_with_post_request(url, data) {
    var form = document.createElement("form");
    form.method = "post";
    form.action = url;
    for (var name in data) {
        var input = document.createElement("input");
        input.type = "hidden";
        input.name = name;
        input.value = data[name];
        form.appendChild(input);
    }
    document.body.appendChild(form);
    form.submit();
}

function force_password_update() {
    var currentUrl = $("#sign_out").find("a").attr("href");
    var redirectTo = $("#user_name").find("a").attr("href") + "/security";
    if (currentUrl === redirectTo) {
        return;
    }
    var data = {
        redirect_to: currentUrl,
        force_pass_update: true
    };
    redirect_with_post_request(redirectTo, data);
}

$(document).on("click", "#close_auth_error", close_auth_error);
$(document).on("click", "#sign_in *", begin_authorization);
$(document).on("click", "#sign_out *", sign_out);
$(document).on("submit", "#auth_form", store_user_name);
$(document).ready(document_postprocessing);

