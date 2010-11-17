# -*- cmake -*-

# The copy_win_libs folder contains file lists and a script used to 
# copy dlls, exes and such needed to run Imprudence from within 
# VisualStudio. 

include(CMakeCopyIfDifferent)

# Copying vivox's alut.dll breaks inworld audio, never use it
set(vivox_src_dir "${CMAKE_SOURCE_DIR}/newview/vivox-runtime/i686-win32")
set(vivox_files
    SLVoice.exe
    #alut.dll
    vivoxsdk.dll
    ortp.dll
    wrap_oal.dll
    )
copy_if_different(
    ${vivox_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/Debug"
    out_targets
    ${vivox_files}
    )
set(all_targets ${all_targets} ${out_targets})

set(debug_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/debug")
set(debug_files
    alut.dll
    openal32.dll
    openjpegd.dll
    libhunspell.dll
    libapr-1.dll
    libaprutil-1.dll
    libapriconv-1.dll
    )

copy_if_different(
    ${debug_src_dir} 
    "${CMAKE_CURRENT_BINARY_DIR}/Debug"
    out_targets 
    ${debug_files}
    )
set(all_targets ${all_targets} ${out_targets})

# Debug config runtime files required for the plugin test mule
set(plugintest_debug_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/debug")
set(plugintest_debug_files
    libeay32.dll
    qtcored4.dll
    qtguid4.dll
    qtnetworkd4.dll
    qtopengld4.dll
    qtwebkitd4.dll
	qtxmlpatternsd4.dll
    ssleay32.dll
    )
copy_if_different(
    ${plugintest_debug_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/../test_apps/llplugintest/Debug"
    out_targets
    ${plugintest_debug_files}
    )
set(all_targets ${all_targets} ${out_targets})

# Debug config runtime files required for the plugin test mule (Qt image format plugins)
set(plugintest_debug_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/debug/imageformats")
set(plugintest_debug_files
    qgifd4.dll
    qicod4.dll
    qjpegd4.dll
    qmngd4.dll
    qsvgd4.dll
    qtiffd4.dll
    )
copy_if_different(
    ${plugintest_debug_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/../test_apps/llplugintest/Debug/imageformats"
    out_targets
    ${plugintest_debug_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${plugintest_debug_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/llplugin/imageformats"
    out_targets
    ${plugintest_debug_files}
    )
set(all_targets ${all_targets} ${out_targets})

# Release & ReleaseDebInfo config runtime files required for the plugin test mule
set(plugintest_release_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/release")
set(plugintest_release_files
    libeay32.dll
    qtcore4.dll
    qtgui4.dll
    qtnetwork4.dll
    qtopengl4.dll
    qtwebkit4.dll
	qtxmlpatterns4.dll
    ssleay32.dll
    libvlc.dll
    libvlccore.dll
    )
copy_if_different(
    ${plugintest_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/../test_apps/llplugintest/Release"
    out_targets
    ${plugintest_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${plugintest_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/../test_apps/llplugintest/RelWithDebInfo"
    out_targets
    ${plugintest_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

# Release & ReleaseDebInfo config runtime files required for the plugin test mule (Qt image format plugins)
set(plugintest_release_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/release/imageformats")
set(plugintest_release_files
    qgif4.dll
    qico4.dll
    qjpeg4.dll
    qmng4.dll
    qsvg4.dll
    qtiff4.dll
    )
copy_if_different(
    ${plugintest_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/../test_apps/llplugintest/Release/imageformats"
    out_targets
    ${plugintest_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${plugintest_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/../test_apps/llplugintest/RelWithDebInfo/imageformats"
    out_targets
    ${plugintest_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${plugintest_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/Release/llplugin/imageformats"
    out_targets
    ${plugintest_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${plugintest_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/llplugin/imageformats"
    out_targets
    ${plugintest_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

# Debug config runtime files required for the plugins
set(plugins_debug_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/debug")
set(plugins_debug_files
    libeay32.dll
    qtcored4.dll
    qtguid4.dll
    qtnetworkd4.dll
    qtopengld4.dll
    qtwebkitd4.dll
	qtxmlpatternsd4.dll
    ssleay32.dll
    )
copy_if_different(
    ${plugins_debug_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/Debug/llplugin"
    out_targets
    ${plugins_debug_files}
    )
set(all_targets ${all_targets} ${out_targets})

# Release & ReleaseDebInfo config runtime files required for the plugins
set(plugins_release_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/release")
set(plugins_release_files
    libeay32.dll
    qtcore4.dll
    qtgui4.dll
    qtnetwork4.dll
    qtopengl4.dll
    qtwebkit4.dll
	qtxmlpatterns4.dll
    ssleay32.dll
    libvlc.dll
    libvlccore.dll
    )
copy_if_different(
    ${plugins_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/Release/llplugin"
    out_targets
    ${plugins_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${plugins_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/llplugin"
    out_targets
    ${plugins_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

set(release_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/release")
set(release_files
    alut.dll
    openal32.dll
    openjpeg.dll
    libhunspell.dll
    libapr-1.dll
    libaprutil-1.dll
    libapriconv-1.dll
    )
    
copy_if_different(
    ${release_src_dir} 
    "${CMAKE_CURRENT_BINARY_DIR}/Release"
    out_targets 
    ${release_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${vivox_src_dir} 
    "${CMAKE_CURRENT_BINARY_DIR}/Release"
    out_targets 
    ${vivox_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${release_src_dir} 
    "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo"
    out_targets 
    ${release_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${vivox_src_dir} 
    "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo"
    out_targets 
    ${vivox_files}
    )
set(all_targets ${all_targets} ${out_targets})

# libvlc plugins
# FIXME we don't need 75% of these may be more, determine which ones we do need
# currently only using this for audio playback

# FIXME no debug plugins

set(vlc_plugins_release_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-win32/lib/release/plugins")
set(vlc_plugins_release_files
    liba52tofloat32_plugin.dll
    liba52tospdif_plugin.dll
    liba52_plugin.dll
    libaccess_attachment_plugin.dll
    libaccess_bd_plugin.dll
    libaccess_fake_plugin.dll
    libaccess_ftp_plugin.dll
    libaccess_http_plugin.dll
    libaccess_imem_plugin.dll
    libaccess_mms_plugin.dll
    libaccess_output_dummy_plugin.dll
    libaccess_output_file_plugin.dll
    libaccess_output_http_plugin.dll
    libaccess_output_shout_plugin.dll
    libaccess_output_udp_plugin.dll
    libaccess_realrtsp_plugin.dll
    libaccess_smb_plugin.dll
    libaccess_tcp_plugin.dll
    libaccess_udp_plugin.dll
    libadjust_plugin.dll
    libadpcm_plugin.dll
    libaes3_plugin.dll
    libaiff_plugin.dll
    libalphamask_plugin.dll
    libaout_directx_plugin.dll
    libaout_file_plugin.dll
    libaout_sdl_plugin.dll
    libaraw_plugin.dll
    libasf_plugin.dll
    libatmo_plugin.dll
    libaudiobargraph_a_plugin.dll
    libaudiobargraph_v_plugin.dll
    libaudioscrobbler_plugin.dll
    libaudio_format_plugin.dll
    libau_plugin.dll
    libavcodec_plugin.dll
    libavi_plugin.dll
    libball_plugin.dll
    libbandlimited_resampler_plugin.dll
    libbda_plugin.dll
    libblendbench_plugin.dll
    libblend_plugin.dll
    libbluescreen_plugin.dll
    libcaca_plugin.dll
    libcanvas_plugin.dll
    libcc_plugin.dll
    libcdda_plugin.dll
    libcdg_plugin.dll
    libchain_plugin.dll
    libchorus_flanger_plugin.dll
    libclone_plugin.dll
    libcolorthres_plugin.dll
    libconverter_fixed_plugin.dll
    libcroppadd_plugin.dll
    libcrop_plugin.dll
    libcvdsub_plugin.dll
    libdeinterlace_plugin.dll
    libdemuxdump_plugin.dll
    libdemux_cdg_plugin.dll
    libdirac_plugin.dll
    libdirect3d_plugin.dll
    libdirectx_plugin.dll
    libdmo_plugin.dll
    libdolby_surround_decoder_plugin.dll
    libdrawable_plugin.dll
    libdshow_plugin.dll
    libdtstofloat32_plugin.dll
    libdtstospdif_plugin.dll
    libdts_plugin.dll
    libdummy_plugin.dll
    libdvbsub_plugin.dll
    libdvdnav_plugin.dll
    libdvdread_plugin.dll
    libequalizer_plugin.dll
    liberase_plugin.dll
    libes_plugin.dll
    libexport_plugin.dll
    libextract_plugin.dll
    libfaad_plugin.dll
    libfake_plugin.dll
    libfilesystem_plugin.dll
    libflacsys_plugin.dll
    libflac_plugin.dll
    libfloat32_mixer_plugin.dll
    libfluidsynth_plugin.dll
    libfolder_plugin.dll
    libfreetype_plugin.dll
    libgaussianblur_plugin.dll
    libgestures_plugin.dll
    libglobalhotkeys_plugin.dll
    libglwin32_plugin.dll
    libgme_plugin.dll
    libgnutls_plugin.dll
    libgoom_plugin.dll
    libgradient_plugin.dll
    libgrain_plugin.dll
    libgrey_yuv_plugin.dll
    libh264_plugin.dll
    libheadphone_channel_mixer_plugin.dll
    libhotkeys_plugin.dll
    libi420_rgb_mmx_plugin.dll
    libi420_rgb_plugin.dll
    libi420_rgb_sse2_plugin.dll
    libi420_yuy2_mmx_plugin.dll
    libi420_yuy2_plugin.dll
    libi420_yuy2_sse2_plugin.dll
    libi422_i420_plugin.dll
    libi422_yuy2_mmx_plugin.dll
    libi422_yuy2_plugin.dll
    libi422_yuy2_sse2_plugin.dll
    libinvert_plugin.dll
    libinvmem_plugin.dll
    libkate_plugin.dll
    liblibass_plugin.dll
    liblibmpeg2_plugin.dll
    liblive555_plugin.dll
    liblogger_plugin.dll
    liblogo_plugin.dll
    liblpcm_plugin.dll
    liblua_plugin.dll
    libmagnify_plugin.dll
    libmarq_plugin.dll
    libmediadirs_plugin.dll
    libmemcpy3dn_plugin.dll
    libmemcpymmxext_plugin.dll
    libmemcpymmx_plugin.dll
    libmirror_plugin.dll
    libmjpeg_plugin.dll
    libmkv_plugin.dll
    libmod_plugin.dll
    libmono_plugin.dll
    libmosaic_plugin.dll
    libmotionblur_plugin.dll
    libmotiondetect_plugin.dll
    libmp4_plugin.dll
    libmpeg_audio_plugin.dll
    libmpgatofixed32_plugin.dll
    libmpgv_plugin.dll
    libmsn_plugin.dll
    libmux_asf_plugin.dll
    libmux_avi_plugin.dll
    libmux_dummy_plugin.dll
    libmux_mp4_plugin.dll
    libmux_mpjpeg_plugin.dll
    libmux_ogg_plugin.dll
    libmux_ps_plugin.dll
    libmux_ts_plugin.dll
    libmux_wav_plugin.dll
    libnetsync_plugin.dll
    libnoise_plugin.dll
    libnormvol_plugin.dll
    libnsc_plugin.dll
    libnsv_plugin.dll
    libntservice_plugin.dll
    libnuv_plugin.dll
    libogg_plugin.dll
    liboldhttp_plugin.dll
    liboldrc_plugin.dll
    liboldtelnet_plugin.dll
    libosdmenu_plugin.dll
    libosd_parser_plugin.dll
    libpacketizer_copy_plugin.dll
    libpacketizer_dirac_plugin.dll
    libpacketizer_flac_plugin.dll
    libpacketizer_h264_plugin.dll
    libpacketizer_mlp_plugin.dll
    libpacketizer_mpeg4audio_plugin.dll
    libpacketizer_mpeg4video_plugin.dll
    libpacketizer_mpegvideo_plugin.dll
    libpacketizer_vc1_plugin.dll
    libpanoramix_plugin.dll
    libparam_eq_plugin.dll
    libplaylist_plugin.dll
    libpng_plugin.dll
    libpodcast_plugin.dll
    libportaudio_plugin.dll
    libpostproc_plugin.dll
    libpsychedelic_plugin.dll
    libps_plugin.dll
    libpuzzle_plugin.dll
    libpva_plugin.dll
    libqt4_plugin.dll
    libquicktime_plugin.dll
    librawaud_plugin.dll
    librawdv_plugin.dll
    librawvideo_plugin.dll
    librawvid_plugin.dll
    librealvideo_plugin.dll
    libreal_plugin.dll
    libremoteosd_plugin.dll
    libripple_plugin.dll
    librotate_plugin.dll
    librss_plugin.dll
    librtp_plugin.dll
    librv32_plugin.dll
    libsap_plugin.dll
    libscaletempo_plugin.dll
    libscale_plugin.dll
    libscene_plugin.dll
    libschroedinger_plugin.dll
    libscreen_plugin.dll
    libsdl_image_plugin.dll
    libsharpen_plugin.dll
    libsimple_channel_mixer_plugin.dll
    libskins2_plugin.dll
    libsmf_plugin.dll
    libspatializer_plugin.dll
    libspdif_mixer_plugin.dll
    libspeex_plugin.dll
    libspudec_plugin.dll
    libstats_plugin.dll
    libstream_filter_rar_plugin.dll
    libstream_filter_record_plugin.dll
    libstream_out_autodel_plugin.dll
    libstream_out_bridge_plugin.dll
    libstream_out_description_plugin.dll
    libstream_out_display_plugin.dll
    libstream_out_dummy_plugin.dll
    libstream_out_duplicate_plugin.dll
    libstream_out_es_plugin.dll
    libstream_out_gather_plugin.dll
    libstream_out_mosaic_bridge_plugin.dll
    libstream_out_raop_plugin.dll
    libstream_out_record_plugin.dll
    libstream_out_rtp_plugin.dll
    libstream_out_smem_plugin.dll
    libstream_out_standard_plugin.dll
    libstream_out_transcode_plugin.dll
    libsubsdec_plugin.dll
    libsubsusf_plugin.dll
    libsubtitle_plugin.dll
    libsvcdsub_plugin.dll
    libswscale_plugin.dll
    libt140_plugin.dll
    libtaglib_plugin.dll
    libtheora_plugin.dll
    libtransform_plugin.dll
    libtrivial_channel_mixer_plugin.dll
    libtrivial_mixer_plugin.dll
    libts_plugin.dll
    libtta_plugin.dll
    libtwolame_plugin.dll
    libty_plugin.dll
    libugly_resampler_plugin.dll
    libvc1_plugin.dll
    libvcd_plugin.dll
    libvideo_filter_wrapper_plugin.dll
    libvisual_plugin.dll
    libvmem_plugin.dll
    libvobsub_plugin.dll
    libvoc_plugin.dll
    libvod_rtsp_plugin.dll
    libvorbis_plugin.dll
    libvout_sdl_plugin.dll
    libvout_wrapper_plugin.dll
    libwall_plugin.dll
    libwaveout_plugin.dll
    libwave_plugin.dll
    libwav_plugin.dll
    libwingdi_plugin.dll
    libx264_plugin.dll
    libxa_plugin.dll
    libxml_plugin.dll
    libxtag_plugin.dll
    libyuvp_plugin.dll
    libyuv_plugin.dll
    libyuy2_i420_plugin.dll
    libyuy2_i422_plugin.dll
    libzip_plugin.dll
    libzvbi_plugin.dll
    )
    
copy_if_different(
    ${vlc_plugins_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/Release/llplugin/plugins"
    out_targets
    ${vlc_plugins_release_files}
    )
set(all_targets ${all_targets} ${out_targets})

copy_if_different(
    ${vlc_plugins_release_src_dir}
    "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/llplugin/plugins"
    out_targets
    ${vlc_plugins_release_files}
    )
set(all_targets ${all_targets} ${out_targets})


# Copy MS C runtime dlls, required for packaging.
# We always need the VS 2005 redist.
# *TODO - Adapt this to support VC9
FIND_PATH(debug_msvc8_redist_path msvcr80d.dll
    PATHS
     [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0\\Setup\\VC;ProductDir]/redist/Debug_NonRedist/x86/Microsoft.VC80.DebugCRT
    NO_DEFAULT_PATH
    NO_DEFAULT_PATH
    )

if(EXISTS ${debug_msvc8_redist_path})
    set(debug_msvc8_files
        msvcr80d.dll
        msvcp80d.dll
        Microsoft.VC80.DebugCRT.manifest
        )

    copy_if_different(
        ${debug_msvc8_redist_path} 
        "${CMAKE_CURRENT_BINARY_DIR}/Debug"
        out_targets 
        ${debug_msvc8_files}
        )
    set(all_targets ${all_targets} ${out_targets})

    set(debug_appconfig_file ${CMAKE_CURRENT_BINARY_DIR}/Debug/${VIEWER_BINARY_NAME}.exe.config)
    add_custom_command(
        OUTPUT ${debug_appconfig_file}
        COMMAND ${PYTHON_EXECUTABLE}
        ARGS
          ${CMAKE_CURRENT_SOURCE_DIR}/build_win32_appConfig.py
          ${CMAKE_CURRENT_BINARY_DIR}/Debug/Microsoft.VC80.DebugCRT.manifest
          ${CMAKE_CURRENT_SOURCE_DIR}/ImprudenceDebug.exe.config
          ${debug_appconfig_file}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Debug/Microsoft.VC80.DebugCRT.manifest
        COMMENT "Creating debug app config file"
        )

endif (EXISTS ${debug_msvc8_redist_path})

FIND_PATH(release_msvc8_redist_path msvcr80.dll
    PATHS
     [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0\\Setup\\VC;ProductDir]/redist/x86/Microsoft.VC80.CRT
    NO_DEFAULT_PATH
    NO_DEFAULT_PATH
    )

if(EXISTS ${release_msvc8_redist_path})
    set(release_msvc8_files
        msvcr80.dll
        msvcp80.dll
        Microsoft.VC80.CRT.manifest
        )

    copy_if_different(
        ${release_msvc8_redist_path} 
        "${CMAKE_CURRENT_BINARY_DIR}/Release"
        out_targets 
        ${release_msvc8_files}
        )
    set(all_targets ${all_targets} ${out_targets})

    copy_if_different(
        ${release_msvc8_redist_path} 
        "${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo"
        out_targets 
        ${release_msvc8_files}
        )
    set(all_targets ${all_targets} ${out_targets})

    set(release_appconfig_file ${CMAKE_CURRENT_BINARY_DIR}/Release/${VIEWER_BINARY_NAME}.exe.config)
    add_custom_command(
        OUTPUT ${release_appconfig_file}
        COMMAND ${PYTHON_EXECUTABLE}
        ARGS
          ${CMAKE_CURRENT_SOURCE_DIR}/build_win32_appConfig.py
          ${CMAKE_CURRENT_BINARY_DIR}/Release/Microsoft.VC80.CRT.manifest
          ${CMAKE_CURRENT_SOURCE_DIR}/Imprudence.exe.config
          ${release_appconfig_file}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Release/Microsoft.VC80.CRT.manifest
        COMMENT "Creating release app config file"
        )

    set(relwithdebinfo_appconfig_file ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/${VIEWER_BINARY_NAME}.exe.config)
    add_custom_command(
        OUTPUT ${relwithdebinfo_appconfig_file}
        COMMAND ${PYTHON_EXECUTABLE}
        ARGS
          ${CMAKE_CURRENT_SOURCE_DIR}/build_win32_appConfig.py
          ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/Microsoft.VC80.CRT.manifest
          ${CMAKE_CURRENT_SOURCE_DIR}/Imprudence.exe.config
          ${relwithdebinfo_appconfig_file}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/RelWithDebInfo/Microsoft.VC80.CRT.manifest
        COMMENT "Creating relwithdebinfo app config file"
        )
      
endif (EXISTS ${release_msvc8_redist_path})

add_custom_target(copy_win_libs ALL
  DEPENDS 
    ${all_targets}
    ${release_appconfig_file} 
    ${relwithdebinfo_appconfig_file} 
    ${debug_appconfig_file}
  )

if(EXISTS ${internal_llkdu_path})
    add_dependencies(copy_win_libs llkdu)
endif(EXISTS ${internal_llkdu_path})
