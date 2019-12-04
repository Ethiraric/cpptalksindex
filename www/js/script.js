var lastqueryinfo = {};

// Load the dropdown lists.
// Calls the API to get the data and creates an entry for each item.
function loadX(url, id, buttonid) {
  jQuery.get(url, function(items, status) {
    for (i = 0; i < items.length; ++i) {
      var item = items[i];
      var node = document.createElement("a");
      node.classList.add("dropdown-item")
      node.id = `${id}-${item}`;
      node.setAttribute("href", "#");
      node.setAttribute("onclick",
        `document.getElementById('${buttonid}').innerHTML = "${item}";`);
      node.innerHTML = item;
      document.getElementById(`${id}-dropdown-list`).appendChild(node);
    }
  });
}

// Display an error message when clicking on "Search" fails.
function setFilterError(errmsg) {
  if (errmsg == "")
    document.getElementById("searcherror").innerHTML = "";
  else
    document.getElementById("searcherror").innerHTML =
    `<div class="alert alert-danger"><center>${errmsg}</center></div>`;
}

// Load the thumbnail for a link.
// TODO(ethiraric): Find a cleaner way to do that..
function loadThumbnail(link) {
  var imagenode = document.createElement("img");
  var uri = "https://img.youtube.com/vi/" + link.slice(-11) + "/2.jpg";
  imagenode.setAttribute("src", uri);
  var anode = document.createElement("a");
  anode.setAttribute("href", link);
  anode.appendChild(imagenode);
  return anode;
}

// Fill the searchresults div.
function showResults(talks) {
  var node = document.getElementById("searchresults");
  node.innerHTML = "";
  if (talks.length == 0) {
    setFilterError("No talk matched your criteria.");
  }
  for (i = 0; i < talks.length; ++i) {
    var talk = talks[i];
    var outerdiv = document.createElement("div");
    outerdiv.classList.add("row", "mb-3", "flex-nowrap");
    outerdiv.appendChild(loadThumbnail(talk.link));
    var descdiv = document.createElement("div");
    descdiv.classList.add("ml-3", "align-self-center");
    descdiv.innerHTML = `<b>${talk.speakers.join(', ')} - ${talk.title}</b></br>
          ${talk.conference} - ${talk.year} </br>
          <a href="${talk.link}">${talk.link}</a>`
    outerdiv.appendChild(descdiv);
    node.appendChild(outerdiv);
  }
}

// Build the URL, call it and display results.
function onSearch() {
  var queryinfo = {
    speaker: document.getElementById('dropdown-speaker').innerHTML.trim(),
    conference: document.getElementById('dropdown-conference').innerHTML
    .trim(),
    year: document.getElementById('dropdown-year').innerHTML.trim(),
    tag: document.getElementById('dropdown-tag').innerHTML.trim()
  };
  if (JSON.stringify(queryinfo) == JSON.stringify(lastqueryinfo)) {
    setFilterError("Same query!");
    return;
  }

  if (queryinfo.speaker == "Speaker" && queryinfo.conference == "Conference" &&
    queryinfo.year == "Year" && queryinfo.tag == "Tag") // no filter
  {
    setFilterError("Please select at least one filter.");
    return;
  }
  setFilterError("");
  var url = '/api/talks?'
  var needsamp = false;

  if (queryinfo.speaker != "Speaker") {
    url += 'speaker=' + queryinfo.speaker;
    needsamp = true;
  }
  if (queryinfo.conference != "Conference") {
    url += (needsamp ? '&' : '') + "conference=" + queryinfo.conference;
    needsamp = true;
  }
  if (queryinfo.year != "Year") {
    url += (needsamp ? '&' : '') + "year=" + queryinfo.year;
    needsamp = true;
  }
  if (queryinfo.tag != "Tag")
    url += (needsamp ? '&' : '') + "tags=" + queryinfo.tag;
  lastqueryinfo = queryinfo;
  jQuery.get(url, function(talks, status) {
    showResults(talks);
  });
}

window.onload = function() {
  // Load each dropdown
  // TODO(ethiraric): add an API that combines all 4.
  loadX('/api/speakers', 'speaker', 'dropdown-speaker');
  loadX('/api/conferences', 'conference', 'dropdown-conference');
  loadX('/api/years', 'year', 'dropdown-year');
  loadX('/api/tags', 'tag', 'dropdown-tag');
}
