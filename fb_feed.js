(function ($) {

//console.log('testi');

Drupal.behaviors.fb_feed = {
	attach: function ( context, settings ) {
		var titleField = $( '#edit-title', '#fb-feed-block-editor' ).add( '#edit-title--2', '#block-admin-configure' );
		titleField.keyup( function() {
			fb_feed_generateName();
		});
		titleField.change( function() {
			fb_feed_generateName();
		});
		
		/*$('.fb-feed-scroller').html('');
		$.ajax({
			url: '?q=admin/structure/fb_feed/block_content',
			success: function (data,status,jq) {
				$('.fb-feed-scroller').each( function() {
					//console.log( data[1].data );
					$(this).html( data[1].data );
				})
			}
		});*/
		
		// makes the loading icons visible while iframes are loading
		fb_feed_show('.fb_loader'); 
	}
};



function fb_feed_generateName() {
	var titleField = $( '#edit-title', '#fb-feed-block-editor' );
	if( titleField.val() != '' ) {
		value = titleField.val()
					.replace( /[ -]/g, '_' )
					.toLowerCase();
		pttrn = /[a-z_]/g;
		value = value.match( pttrn );
		$( '#edit-name', '#fb-feed-block-editor' ).val( value.join('') ); 
	} else {
		$( '#edit-name', '#fb-feed-block-editor' ).val('');
	}
}
  
function fb_feed_hide() {}

window.fb_feed_show = function(id) {
	$(id).show();
}
window.fb_feed_hide = function(id) {
	$(id).hide();
} 
  
})(jQuery);