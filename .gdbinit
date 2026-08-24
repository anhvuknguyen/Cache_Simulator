# .gdbinit for Cache_Simulator
# Place in the project root. Requires the following line in ~/.gdbinit:
#   add-auto-load-safe-path /mnt/c/Projects/PersonalProjects/Cache_Simulator

break main.cpp:268

define advance_Trace
  continue
  printf "%s", cache_h->viewCache().c_str()
end

document advance_Trace
Continue to the next breakpoint hit, then print the cache contents.
end

define view_Cache
  printf "%s", cache_h->viewCache().c_str()
end

document view_Cache
Print the current contents of the cache hierarchy.
end

define view_Cache_stats
  printf "%s", cache_h->getStats().c_str()
end

document view_Cache_stats
Print the cache hierarchy statistics.
end
