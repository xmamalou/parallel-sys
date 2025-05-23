Στην άσκηση αυτήν υλοποιούμε το Παιχνίδι της Ζωής (Conway's Game of Life) του John Conway, σε σειριακή και παράλληλη μορφή (χρησιμοποιώντας την OpenMP). Το παιχνίδι περιλαμβάνει έναν πίνακα κυττάρων τα οποία ζουν ή πεθαίνουν βάσει ορισμένων κανόνων. Παρακάτω παρατίθεται μία εικόνα με τους κανόνες του παιχνιδιού (@gol_rules):

#figure(
  image("res/img/gol_rules.png", width: 80%),
  caption: [Κανόνες του Παιχνιδιού της Ζωής. Ο συμβολισμός $Α > Β$ σημαίνει ότι το κεντρικό κύτταρο μεταβαίνει από την κατάσταση $Α$ στην $Β$. Τα πράσινα κύτταρα περί αυτού δηλώνουν ζωντανά κύτταρα. $Ζ$ είναι η ζωντανή κατάσταση και $Ν$ η νεκρή.],
) <gol_rules>

Ο πηγαίος κώδικας της άσκησης βρίσκεται στο αρχείο `src/game_of_life.c`.

Η άσκηση δέχεται ως ορίσματα το πλήθος των γεννεών για τις οποίες θα τρέξει το παιχνίδι (`-fg=<number>`), το πλήθος των νημάτων που θα χρησιμοποιηθούν (`-fj=<number>`), το μέγεθος του πίνακα (`-fm=<number>`, ο πίνακας είναι τετραγωνικός), και αν θα χρησιμοποιηθεί η σειριακή ή η παράλληλη υλοποίηση (`-fs` για την σειριακή και `-fp` για την παράλληλη). Η έξοδος περιέχει τον χρόνο εκτέλεσης του αλγορίθμου, όπως και την υλοποίηση που χρησιμοποιήθηκε αλλά και το πλήθος των γεννεών και το μέγεθος του πίνακα, η οποία προεραιτικά μπορεί να αποθηκευτεί σε αρχείο με το όρισμα `-ff=<filename>`.

Θεωρήσαμε τον αριθμό γεννεών σταθερό και ίσο με $1000$. Τα αποτελέσματα της άσκησης είναι τα εξής:
#image("res/img/plot6_serial.png", width: 100%, height: 20%)
#image("res/img/plot6_parallel.png", width: 100%, height: 20%)

Βλέπουμε ότι μόλις βάλουμε τον αλγόριθμο να εκτελείται από δύο νήματα, ο χρόνος υλοποίησης μειώνεται στο μισό. Στα 8 νήματα, ο χρόνος εκτέλεσης είναι περίπου 1/4 του χρόνου εκτέλεσης της σειριακής υλοποίησης. Αυτό συμβαίνει γιατί ο αλγόριθμος είναι πολύ απλός και δεν έχει καθόλου συγχρονισμό (τα νήματα δεν γράφουν στον ίδιο πίνακα που αναγιγνώσκουν). Το ενδιαφέρον εδώ είναι το γεγονός ότι παρά το ότι το σύστημα διαθέτει 4 φυσικούς πυρήνες μόνο, έχουμε παρ`όλα αυτά βελτίωση του χρόνου εκτέλεσης όταν υπερβούμε τα 4 νήματα.