function add_item_into_projects_table(i, item) {
    var html = "<li><a id=\"" + item.project_name + 
               "\" href=\"%prefix%projects/" + item.project_name +
               "\">" + item.project_name + "</a></li>\n";
    $('#projects_table').append(html);
}

function fill_in_projects_table(data) {
    data = $.parseJSON(data);
    $.each(data, add_item_into_projects_table);
}

function load_projects_table() {
    $.post("./?ajax=content", "", fill_in_projects_table);
}

$(document).ready(load_projects_table);

