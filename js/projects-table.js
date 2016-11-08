// License: MIT (Expat)

function add_item_into_projects_table(i, item) {
    num = +localStorage.projects_number;
    localStorage.projects_number = num + 1;

    last_status = "?";
    if (item.last_status === "passed")
        last_status = "✔";
    else if (item.last_status === "failed")
        last_status = "✘";
    else if (item.last_status === "started")
        last_status = "⚫";

    html =  "<div id=\"" + item.project_name + "\" class=\"table_item " +
            item.last_status + "\">\n" +
                "<li class=\"last_status\" title=\"" +
                    $("#" + item.last_status).text() + "\">\n" + last_status +
                "</li>\n" +
                "<li class=\"project_name\">\n" +
                    "<a class=\"" + item.last_status + "\" " + "href=\"" +
                    $("#projects_page").find("a").attr("href") +
                    "/" + item.project_name + "\">" + item.project_name + "</a>\n" +
                "</li>\n" +
                "<li class=\"builds_number\">\n" + item.builds_number + "</li>\n" +
                "<li class=\"last_version\">\n" +
                    "<a class=\"" + item.last_status + "\" " + "href=\"" +
                    $("#projects_page").find("a").attr("href") +
                    "/" + item.project_name + "/" + item.last_version + "\">" +
                    item.last_version + "</a>\n" +
                "</li>\n" +
                "<li class=\"last_timestamp\">\n" + item.last_timestamp + "</li>\n" +
                "<li class=\"role\">\n" + $("#" + item.role).text() + "</li>\n" +
            "</div>\n";
    $("#projects_table").append(html);

    if (item.hasOwnProperty("last_project")) {
        if (item.last_project) {
            $("#show_more").remove();
        }
    }
}

function fill_in_projects_table(data) {
    array = $.parseJSON(data);
    $.each(array, add_item_into_projects_table);
}

function show_more_projects() {
    len = +localStorage.projects_per_page;
    pos = +localStorage.projects_number;
    data = "pos=" + pos + "&len=" + len;
    $.post("./?ajax=projects_list", data, fill_in_projects_table);
    return false;
}

function load_projects_table() {
    localStorage.projects_per_page = 7;
    localStorage.projects_number = 0;
    show_more_projects();
}

$(document).on("click", "#show_more", show_more_projects);
$(document).ready(load_projects_table);

